/* @COPYRIGHT@ */

#include "../fastjson/fastjson.hxx"
#include "../compression.hxx"

/* Import an mae file into a json.  Both blocks and array blocks are 
 * represented as objects (dictionaries); array blocks are dictionaries
 * of lists, while regular blocks are dictionaries of atoms, arrays,
 * and other blocks.  Array blocks are distinguished from regular blocks
 * by the presence of a __size__ member.  The first block at the top level 
 * is the "meta" * block; the rest are "f_m_ct" blocks.  So we have 
 * something like:
 
 [
    { "m_m2io_version" : "2.0.0" },
    { "m_title" : "title in water",
      "chorus_box_ax" : 25.0,
      "m_atom" : {
        "__size__" : 3,
        "m_x_coord" : [ 1,2,3 ],
        "m_y_coord" : [ 4,5,6 ] },
      "ffio_ff" : {
        "ffio_name" : "OPLS",
        "ffio_vdwtypes" : {
            "__size__" : 2,
            "ffio_funct" : [ "LJ12_6_sig_epsilon", "LJ12_6_sig_epsilon"],
            "ff_c1" : [ 3.5, 2.5 ] }
        }
    }
]
     

 */

namespace desres { namespace msys { namespace mae {

    using fastjson::Json;

    void import_mae( std::istream& in, Json& js );

    struct tokenizer;

    class import_iterator {
        std::unique_ptr<std::istream> in;
        tokenizer* tk;
        std::streamsize _offset;

    public:
        explicit import_iterator(std::istream& file);
        ~import_iterator();

        /* read the next ct block; return true on success or false on EOF */
        bool next(Json& js);
        std::streamsize offset() const { return _offset; }
    };

}}}

