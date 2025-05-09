"""
dtr-validate input.{dtr,atr,etr,stk}

Check that each frame in each input file is readable and provides
position and box data.
"""
import sys, os, fnmatch
from msys import molfile
import numpy
import math
import threading
import multiprocessing


class Progress(object):
    def __init__(self, total, dots=50, nums=11):
        points = list(map(int, numpy.linspace(1, total, dots)))
        progs = list(map(int, numpy.linspace(0, 100, dots)))
        self.dots = dict(zip(points, progs))
        points = list(map(int, numpy.linspace(1, total, nums)))
        progs = list(map(int, numpy.linspace(0, 100, nums)))
        self.nums = dict(zip(points, progs))
        self.total = total

    def __call__(self, i):
        p = self.nums.get(i)
        if p is not None:
            sys.stderr.write("%d%%" % p)
        p = self.dots.get(i)
        if p is not None:
            sys.stderr.write(".")
        if i == self.total:
            sys.stderr.write("\n")
        sys.stderr.flush()


def parse_args():
    import argparse

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input_path", type=str, help="dtr/atr/etr/stk to validate")
    parser.add_argument(
        "--progress", action="store_true", default=False, help="show progress bar"
    )
    parser.add_argument(
        "--checkpoint",
        action="store_true",
        default=False,
        help="turn off delta_t checks",
    )
    parser.add_argument(
        "--energy",
        action="store_true",
        default=False,
        help="turn off box, pos and atom based checks",
    )
    parser.add_argument(
        "--parseable-output",
        action="store_true",
        default=False,
        help="show parseable output compatible with d_validate",
    )
    parser.add_argument(
        "--parallelism",
        type=int,
        default=int(math.ceil(multiprocessing.cpu_count()/2.0)),   # use 1/2 the cores, round up to never get 0
        help="Number of frames to process in parallel",
    )

    return parser.parse_args()

class FrameLoadThread(threading.Thread):
    def __init__(self, reader, frameno):
        super().__init__()
        self.reader = reader
        self.frameno = frameno

    def run(self):
        self.frame = self.reader.frame(self.frameno)

def main():
    args = parse_args()
    path = args.input_path
    r = molfile.DtrReader(path, sequential=False)    # multi-threaded - need to re-open on each access

    if os.path.isdir(path):
        total_framebytes = 0
        for framefile in os.listdir(path):
            if fnmatch.fnmatch(
                framefile, "frame[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"
            ):
                total_framebytes += os.path.getsize(os.path.join(path, framefile))

        assert total_framebytes == r.total_bytes(), (
            "total frame bytes %d vs expected from timekeys file %d"
            % (total_framebytes, r.total_bytes(),)
        )

    natoms = r.natoms
    nframes = r.nframes
    if args.progress:
        print("frames: %-9d atoms: %-9d" % (nframes, natoms))
    if not args.energy:
        assert natoms > 0, "'%s' contains %d atoms." % (path, natoms)
    assert nframes > 0, "'%s' contains %d frames." % (path, nframes)
    if args.progress:
        progress = Progress(nframes)

    for start in range(0, nframes, args.parallelism):
        # Read up to args.parallelism frames at a time in separate threads
        frame_reader_threads = []
        for j in range(start, nframes)[:args.parallelism]:
            frame_reader_threads.append(FrameLoadThread(r, j))
            frame_reader_threads[-1].start()
        for j in range(len(frame_reader_threads)):
            # Wait till frame read is finished, then grab the completed frame
            frame_reader_threads[j].join()
            frame = frame_reader_threads[j].frame
            i = start + j
            if not args.energy:
                assert frame.pos is not None, "'%s' is missing pos data at frame %d" % (
                    path,
                    i,
                )
                assert frame.box is not None, "'%s' is missing pos data at frame %d" % (
                    path,
                    i,
                )
            if args.progress:
                progress(i + 1)

            if i == 0:
                first_frame_time = frame.time
            else:
                delta_t = frame.time - last_frame_time
                if i > 1:
                    change_delta_t = abs(last_delta_t - delta_t)
                    if args.checkpoint == False:
                        assert change_delta_t < 0.02, "delta_t changed from %f to %f" % (
                            last_delta_t,
                            delta_t,
                        )

                last_delta_t = delta_t

            last_frame_time = frame.time

    #
    # Spit out some output that is compatible with d_validate, which
    # this replaces.
    #
    if args.parseable_output:
        print("first_frame_time = ", first_frame_time)
        print("last_frame_time = ", frame.time)
        if nframes > 1:
            print("delta_t = ", (frame.time - first_frame_time) / (nframes - 1))
            print("num_frames = ", nframes)
