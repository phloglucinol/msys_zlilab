import os
import subprocess
import sys
import sqlite3
import tempfile

dump_doc = """
dmsdump [ options ] input.dms

dms-dump generates a textual representation of a dms file that can be
understood by humans and compared to the output of another dms file.
A certain amount of canonicalization is applied to the contents of the dms
file in order to make this happen:

 * all floating point values are rounded to six decimals;

 * all force terms are sorted by particle id, i.e. p0, p1, ...

 * all columns are sorted alphabetically;

 * rather than printing the nbtype of each particle, the nonbonded parameters
   of each particle are dumped with a "nonbonded" label at the start of
   each line.
"""

diff_opts = "--without-provenance --without-forcefield --without-paraminfo --reorder"
diff_doc = (
    """
Writes to stdout a Unix diff of the result of running dms-dump
on ``file1.dms`` and ``file2.dms``.  The environment variable
``DMSDIFF`` can be used to specify an alternate file comparison
utility.

The options %s
will be passed along to dms-dump, along with any other arguments
after the inputs files.
"""
    % diff_opts
)


def call_sqlite3(cmd, stdout=sys.stdout, stderr=None):
    p = subprocess.Popen(
        cmd, universal_newlines=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE
    )
    p_stdout, p_stderr = p.communicate()

    def should_remove_line(line):
        if line.startswith("-- Loading resource from"):
            return True
        return False

    print(
        "\n".join([l for l in p_stdout.splitlines() if not should_remove_line(l)]),
        file=stdout,
    )
    if stderr is not None:
        print(p_stderr, file=stderr)


def read_terms(conn, table):
    cur = conn.cursor()
    try:
        cur.execute("select name from %s order by name" % table)
    except sqlite3.OperationalError:
        return []
    return [str(x[0]) for x in cur]

    # count the number of p0, p1, ... columns in the table


def get_columns(conn, table):
    fetched = conn.execute("pragma table_info(%s)" % table).fetchall()
    atoms = []
    params = []
    for elems in fetched:
        elems = list(map(str, elems))
        if len(elems) < 2:
            continue
        col = elems[1]
        if col and col[0] == "p":
            try:
                int(col[1:])
            except:
                pass
            else:
                atoms.append(col)
                continue
        params.append(col)

    return atoms, params


def has_table(conn, table):
    tables = [
        str(r[0]) for r in conn.execute("select name from sqlite_master").fetchall()
    ]
    return table in tables


def newline(out):
    # out.flush()
    # print >> out
    # out.flush()
    pass


def dump_terms(cmd, ipath, conn, out, table, without_paraminfo, reorder):
    terms = read_terms(conn, table)
    terms.sort()
    for b in terms:
        atoms, params = get_columns(conn, b)
        # don't print moiety for compatibility with ent output
        params = [p for p in params if p != "moiety"]
        if without_paraminfo:
            exclude = set(
                (
                    "memo",
                    "comment",
                    "ff",
                    "type",
                    "typekey",
                    "memoA",
                    "commentA",
                    "ffA",
                    "typeA",
                    "typekeyA",
                    "memoB",
                    "commentB",
                    "ffB",
                    "typeB",
                    "typekeyB",
                    "annotation",
                    "qij_annotation"
                )
            )
            params = [p for p in params if p not in exclude]

        if "constrained" in params:
            params = [p for p in params if p != "constrained"]
            params.sort()
            params.append("constrained")
        else:
            params.sort()

        a = ",".join(atoms)
        s = ",".join(atoms + params)
        sql = 'select "%s", "%s", %s from %s' % (table, b, s, b)
        if reorder:
            sql += " order by %s" % a
        call_sqlite3(cmd + [sql], stdout=out)
        newline(out)


def dmsdump(
    ipath,
    out,
    without_provenance=False,
    without_groups=False,
    without_forcefield=False,
    without_paraminfo=False,
    without_pos=False,
    reorder=False,
):
    if not os.path.isfile(ipath):
        raise IOError("No dms file at '%s'" % ipath)
    cmd = "sqlite3 -header".split()
    cmd.append(ipath)
    conn = sqlite3.connect(ipath)

    # global cell
    sql = 'select "global_cell", x,y,z from global_cell'
    call_sqlite3(cmd + [sql], stdout=out)

    # particles
    # we handle nonbonded specially so that permutations of the
    # nbtype don't affect the result.
    atoms, params = get_columns(conn, "particle")
    skipped = set(
        (
            "nbtype",
            "nbtypeB",
            "bfactor",
            "occupancy",
            "formal_charge",
            "resonant_charge",
        )
    )
    params = [p for p in params if p not in skipped]
    if without_groups:
        params = [p for p in params if not p.startswith("grp_")]
    if without_pos:
        params = [p for p in params if p not in ["x", "y", "z", "vx", "vy", "vz"]]
    if "segid" not in params:
        params.append("segid")
        fake_segid = True
    else:
        fake_segid = False
    if "name" not in params:
        params.append("name")
        fake_name = True
    else:
        fake_name = False
    if "resname" not in params:
        params.append("resname")
        fake_resname = True
    else:
        fake_resname = False
    params.sort()
    if fake_segid:
        params[params.index("segid")] = '"" as segid'
    if fake_name:
        params[params.index("name")] = '"" as name'
    else:
        params[params.index("name")] = "trim(name) as name"
    if fake_resname:
        params[params.index("resname")] = '"" as resname'
    else:
        params[params.index("resname")] = "trim(resname) as resname"

    s = ",".join(params)

    sql = 'select "particle", %s from particle' % s

    call_sqlite3(cmd + [sql], stdout=out)
    newline(out)

    # bonds
    atoms, params = get_columns(conn, "bond")
    if "order" in params:
        call_sqlite3(
            cmd + ['select "bond", p0,p1, "order" from bond order by p0, p1'],
            stdout=out,
        )
    else:
        call_sqlite3(
            cmd + ['select "bond", p0,p1, 1.0 as "order" from bond order by p0, p1'],
            stdout=out,
        )
    newline(out)

    # bond, constraint, and virtual and drude terms
    dump_terms(cmd, ipath, conn, out, "bond_term", without_paraminfo, reorder)
    dump_terms(cmd, ipath, conn, out, "constraint_term", without_paraminfo, reorder)
    dump_terms(cmd, ipath, conn, out, "virtual_term", without_paraminfo, reorder)
    dump_terms(cmd, ipath, conn, out, "polar_term", without_paraminfo, reorder)
    dump_terms(cmd, ipath, conn, out, "nonbonded_table", without_paraminfo, reorder)

    # cmap, if present
    for c in range(1, 100):
        table = "cmap%d" % c
        if not has_table(conn, table):
            continue
        sql = 'select "%s", phi,psi,energy from %s' % (table, table)
        call_sqlite3(cmd + [sql], stdout=out)

    # nonbonded
    if has_table(conn, "nonbonded_info"):
        sql = 'select "nonbonded_info", vdw_funct, vdw_rule from nonbonded_info'
        call_sqlite3(cmd + [sql], stdout=out)

    if has_table(conn, "nonbonded_param"):
        atoms, params = get_columns(conn, "nonbonded_param")
        if without_paraminfo:
            for p in (
                "ff",
                "type",
                "typekey",
                "memo",
                "comment",
                "override",
                "ffA",
                "typeA",
                "typekeyA",
                "memoA",
                "commentA",
                "overrideA",
                "ffB",
                "typeB",
                "typekeyB",
                "memoB",
                "commentB",
                "overrideB",
                "q_annotation",
                "exch_annotation",
                "disp_annotation"
            ):
                if p in params:
                    params.remove(p)
        params.remove("id")
        params.sort()

        s = ""
        if not without_paraminfo:
            s = "nb.id as param, "
        s += ",".join(["nb.%s as %s" % (p, p) for p in params])
        sql = (
            'select "nonbonded", p.id, %s from particle as p join nonbonded_param as nb'
            " on p.nbtype = nb.id"
            % s
        )
        call_sqlite3(cmd + [sql], stdout=out)

    # nonbonded combined
    if has_table(conn, "nonbonded_combined_param"):
        atoms, params = get_columns(conn, "nonbonded_combined_param")
        params.remove("param1")
        params.remove("param2")
        params.sort()
        params = ['"nonbonded_combined"', "param1", "param2"] + params
        s = ",".join(params)
        sql = "select %s from nonbonded_combined_param" % s
        call_sqlite3(cmd + [sql], stdout=out)

    # modified_interaction
    if has_table(conn, "modified_interaction"):
        sql = (
            'select "modified_interaction", * from modified_interaction order by g0, g1'
        )
        call_sqlite3(cmd + [sql], stdout=out)

    # tempering_groups
    if has_table(conn, "tempering_groups"):
        sql = (
            'select "tempering_groups", rowid, * from tempering_groups'
        )
        call_sqlite3(cmd + [sql], stdout=out)

    # tempered_constraint
    if has_table(conn, "tempered_constraint"):
        sql = (
            'select "tempered_constraint", rowid, * from tempered_constraint'
        )
        call_sqlite3(cmd + [sql], stdout=out)

    # exclusions
    if has_table(conn, "exclusion"):
        sql = 'select "exclusion", * from exclusion'
        if reorder:
            sql += " order by p0,p1"
        call_sqlite3(cmd + [sql], stdout=out)

    # alchemical_particle
    if has_table(conn, "alchemical_particle"):
        s = ",".join(["nbA.%s as %sA, nbB.%s as %sB" % (p, p, p, p) for p in params])
        sql = (
            """
        select "alchemical_particle", p.p0 as p0, chargeA, chargeB, %s
        from alchemical_particle as p
        join nonbonded_param as nbA on p.nbtypeA = nbA.id
        join nonbonded_param as nbB on p.nbtypeB = nbB.id
        """
            % s
        )
        call_sqlite3(cmd + [sql], stdout=out, stderr=sys.stderr)

    # viparr_info, forcefield
    for blk in ("provenance", "viparr_info", "forcefield"):
        if blk == "provenance" and without_provenance:
            continue
        if blk == "forcefield" and without_forcefield:
            continue
        if has_table(conn, blk):
            atoms, params = get_columns(conn, blk)
            params.sort()
            sql = 'select "%s", %s from %s' % (blk, ",".join(params), blk)
            call_sqlite3(cmd + [sql], stdout=out)


def parser():
    from argparse import ArgumentParser

    parser = ArgumentParser()
    parser.add_argument(
        "--without-provenance", help="don't write provenance", action="store_true"
    )
    parser.add_argument(
        "--without-groups", help="don't write grp_xxx columns", action="store_true"
    )
    parser.add_argument(
        "--without-forcefield", help="don't write forcefield info", action="store_true"
    )
    parser.add_argument(
        "--without-paraminfo",
        help="don't write memo, type, ff, comment, or typekey in terms",
        action="store_true",
    )
    parser.add_argument(
        "--without-pos", help="don't write pos and vel", action="store_true"
    )
    parser.add_argument(
        "--reorder", action="store_true", help="order ouput of terms by particle id"
    )
    return parser


def main():
    prog = os.path.basename(sys.argv[0])
    p = parser()
    if prog.startswith("dms-dump"):
        p.add_argument("ifile", help="structure file")
        args = p.parse_args()
        return dump_main(**args.__dict__)
    elif prog.startswith("dms-diff"):
        p.add_argument("ifiles", nargs=2, help="structure file")
        p.add_argument(
            "-q", "--quiet", action="store_true", help="report only when files differ"
        )
        args = p.parse_args()
        kwds = dict(
            without_provenance=True,
            without_forcefield=True,
            without_paraminfo=True,
            reorder=True,
        )
        for k, v in args.__dict__.items():
            if v:
                kwds[k] = v
        return diff_main(**kwds)


def dump_main(ifile, **kwds):
    return dmsdump(ifile, sys.stdout, **kwds)


def diff_main(ifiles, quiet=False, **kwds):
    out1 = tempfile.NamedTemporaryFile(mode="w+")
    out2 = tempfile.NamedTemporaryFile(mode="w+")
    dmsdump(ifiles[0], out1, **kwds)
    dmsdump(ifiles[1], out2, **kwds)
    out1.flush()
    out2.flush()
    diff = os.getenv("DMSDIFF", "diff")
    stdout = tempfile.NamedTemporaryFile(mode="w+") if quiet else None
    return subprocess.call(["diff", out1.name, out2.name], stdout=stdout)
