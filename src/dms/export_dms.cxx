#include "dms.hxx"
#include "../dms.hxx"
#include "../term_table.hxx"
#include "../override.hxx"
#include "../append.hxx"
#include "../compression.hxx"

#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

using namespace desres::msys;

static const char* str(const ValueType& t) {
    return t==IntType   ? "integer" :
           t==FloatType ? "float" :
                          "text";
}

static void write(const ValueRef& ref, int col, Writer w) {
    switch (ref.type()) {
        case IntType: 
            w.bind_int(col, ref.asInt()); 
            break;
        case FloatType: 
            w.bind_flt(col, ref.asFloat()); 
            break;
        default:
        case StringType:
            w.bind_str(col, ref.asString()); 
            break;
    }
}

/* Return map from atom id to nonbonded param, or empty table if the
 * nonbonded table is not present. */
IdList fetch_nbtypes(const System& sys) {
    TermTablePtr table = sys.table("nonbonded");
    if (!table) return IdList();
    IdList nb(sys.maxAtomId(), BadId);
    for (Id i=0; i<table->maxTermId(); i++) {
        if (!table->hasTerm(i)) continue;
        Id param = table->param(i);
        Id atom = table->atom(i,0);
        if (!table->params()->hasParam(param)) {
            MSYS_FAIL("Cannot export system to DMS file: particle " << atom
                   << " has invalid nonbonded param");
        }
        if (nb.at(atom)!=BadId) {
            MSYS_FAIL("Cannot export system to DMS file: nonbonded table "
                   << "references particle " << atom << "multiple times");
        }
        nb.at(atom) = param;
    }
    return nb;
}

static void export_alchemical_particles(const System& sys, 
                                        IdList const& nbtypes,
                                        Sqlite dms) {

    TermTablePtr alc = sys.table("alchemical_nonbonded");
    if (!alc || alc->termCount()==0) return;

    std::stringstream ss;
    ss << "create table alchemical_particle (\n"
       << "  p0 integer primary key,\n"
       << "  chargeA float,\n";
    for (Id i=0; i<alc->termPropCount(); i++) {
        ss << "'" << alc->termPropName(i) << "' " 
           << str(alc->termPropType(i)) << ", ";
    }
    ss << "  nbtypeA integer, \n"
       << "  nbtypeB integer);\n";

    dms.exec(ss.str());
    Writer w = dms.insert("alchemical_particle");
    for (Id i=0; i<alc->maxTermId(); i++) {
        if (!alc->hasTerm(i)) continue;
        Id id = alc->atom(i,0);
        Id param = alc->param(i);
        if (!alc->params()->hasParam(param)) {
            MSYS_FAIL("alchemical particle " << id << " has invalid nonbonded parameter");
        }
        const atom_t& atom = sys.atom(id);
        w.bind_int(0,id);
        w.bind_flt(1,atom.charge);
        for (Id j=0; j<alc->termPropCount(); j++) {
            write(alc->termPropValue(i,j), 2+j, w);
        }
        w.bind_int(2+alc->termPropCount(), nbtypes[id]);
        w.bind_int(3+alc->termPropCount(), param);
        w.next();
    }
}

static void export_particles(const System& sys, const IdList& map, Sqlite dms,
        bool structure_only) {

    IdList nbtypes = structure_only ? IdList() : fetch_nbtypes(sys);
    // check for bad ids here, rather than inside a sqlite transaction.
    auto nb_bad = std::find(nbtypes.begin(), nbtypes.end(), BadId);
    if (nb_bad != nbtypes.end()) {
        auto id = std::distance(nbtypes.begin(), nb_bad);
        MSYS_FAIL("Missing nonbonded param for particle " << id);
    }

    IdList ids = sys.atoms();

    std::string sql = 
        "create table particle (\n"
        "  id integer primary key,\n"
        "  anum integer,\n"
        "  name text not null,\n" 
        "  x float,\n"
        "  y float,\n"
        "  z float,\n"
        "  vx float,\n"
        "  vy float,\n"
        "  vz float,\n"
        "  resname text not null,\n"
        "  resid integer,\n"
        "  chain text not null,\n"
        "  segid text not null,\n"
        "  mass float,\n"
        "  charge float,\n" 
        "  formal_charge integer,\n"
        "  insertion text not null,\n"
        "  msys_ct integer not null,\n";

    const Id nprops = sys.atomPropCount();
    for (Id i=0; i<nprops; i++) {
        sql += "  '" + sys.atomPropName(i) + "' ";
        sql += str(sys.atomPropType(i));
        sql += ",\n";
    }
    if (nbtypes.size()) {
        sql += "  nbtype integer not null\n);";
    } else {
        sql.resize(sql.size()-2);
        sql += ");";
    }
    dms.exec( sql.c_str());

    Writer w = dms.insert("particle");
    dms.exec("begin");
    for (Id i=0, n=ids.size(); i<n; i++) {
        Id atm = ids[i];
        const atom_t& atom = sys.atom(atm);
        Id res = atom.residue;
        const residue_t& residue = sys.residue(res);
        Id chn = residue.chain;
        const chain_t& chain = sys.chain(chn);

        w.bind_int( 0, map[atm]);
        w.bind_int( 1, atom.atomic_number);
        w.bind_str( 2, atom.name.c_str());
        w.bind_flt( 3, atom.x);
        w.bind_flt( 4, atom.y);
        w.bind_flt( 5, atom.z);
        w.bind_flt( 6, atom.vx);
        w.bind_flt( 7, atom.vy);
        w.bind_flt( 8, atom.vz);
        w.bind_str( 9, residue.name.c_str());
        w.bind_int(10, residue.resid);
        w.bind_str(11, chain.name.c_str());
        w.bind_str(12, chain.segid.c_str());
        w.bind_flt(13, atom.mass);
        w.bind_flt(14, atom.charge);
        w.bind_int(15, atom.formal_charge);
        w.bind_str(16, residue.insertion.c_str());
        w.bind_int(17, chain.ct);

        for (Id j=0; j<nprops; j++) {
            int col=18+j;

            /* *sigh* - the ParamTable::value() method is non-const,
             * and I don't feel like making a const version; thus this
             * hack. */
            ValueRef ref = const_cast<System&>(sys).atomPropValue(atm,j);
            write(ref, col, w);
        }
        if (nbtypes.size()) {
            Id param = nbtypes.at(atm);
            w.bind_int(18+nprops,param);
        }
        try {
            w.next();
        }
        catch (std::exception& e) {
            std::stringstream ss;
            ss << "Error writing particle table for atom id " << atm 
               << " gid " << map[atm] << ": " << e.what();
            throw std::runtime_error(ss.str());
        }
    }
    export_alchemical_particles(sys, nbtypes, dms);
    dms.exec( "commit");
}

static void export_bonds(const System& sys, const IdList& map, Sqlite dms) {
    std::string sql =
        "create table bond (\n"
        "  p0 integer,\n"
        "  p1 integer,\n"
        "  'order' integer\n"
        ");";
    dms.exec( sql.c_str());

    Writer w = dms.insert("bond");
    IdList ids = sys.bonds();
    dms.exec( "begin");
    for (Id i=0, n=ids.size(); i<n; i++) {
        Id bnd = ids[i];
        const bond_t& bond = sys.bond(bnd);
        w.bind_int(0,map[bond.i]);
        w.bind_int(1,map[bond.j]);
        w.bind_int(2,bond.order);
        w.next();
    }
    dms.exec( "commit");
}

static void export_terms(TermTablePtr table, const IdList& map, 
                         const std::string& tablename, Sqlite dms) {

    const Id natoms = table->atomCount();
    const Id nprops = table->termPropCount();
    std::stringstream ss;
    ss << "create table " << tablename << " (";
    for (Id i=0; i<natoms; i++) ss << "p" << i << " integer, ";
    for (Id i=0; i<nprops; i++) {
        ss << "'" << table->termPropName(i) << "' " 
           << str(table->termPropType(i)) << ", ";
    }
    ss << "param integer not null)";
    dms.exec( ss.str().c_str());
    ParamTablePtr params = table->params();

    Writer w = dms.insert(tablename);
    dms.exec("begin");
    IdList ids = table->terms();
    for (Id i=0,n=ids.size(); i<n; i++) {
        Id id=ids[i];

        /* write atom columns */
        IdList atoms = table->atoms(id);
        for (Id j=0; j<natoms; j++) w.bind_int(j,map[atoms[j]]);
        /* write extra atom properties */
        for (Id j=0; j<nprops; j++) {
            ValueRef val = table->termPropValue(id, j);
            write(val, j+natoms, w);
        }
        /* write param column.  We refuse to write null params to a DMS file! */
        Id param = table->param(id);
        if (bad(param) || !params->hasParam(param)) {
            std::stringstream ss;
            ss << "Cannot write DMS file: table '" << tablename << "' termid "
                << id << " has missing or invalid param";
            throw std::runtime_error(ss.str());
        } else {
            w.bind_int(nprops+natoms,param);
        }
        w.next();
    }
    dms.exec("commit");
}

static void export_params(ParamTablePtr params, const std::string& tablename,
                          Sqlite dms, bool with_id=true) {

    std::stringstream ss;
    const Id nprops=params->propCount();
    if (nprops==0 && !with_id) {
      /* no columns have been specified, so there are no relations and thus
       * nothing to do.  */
      return;
    }
    ss << "create table " << tablename << " (";
    for (Id i=0; i<nprops; i++) {
        ss << "'" << params->propName(i) << "' " << str(params->propType(i));
        if (i!=nprops-1) ss << ", ";
    }
    if (with_id) {
        if (nprops>0) ss << ", ";
        ss << "id integer primary key)";
    } else {
        ss << ")";
    }

    dms.exec( ss.str().c_str());
    Writer w = dms.insert(tablename);
    dms.exec( "begin");
    for (Id i=0, n=params->paramCount(); i<n; i++) {
        for (Id j=0; j<nprops; j++) {
            ValueRef val = params->value(i,j);
            write(val, j, w);
        }
        if (with_id) {
            w.bind_int(nprops,i);
        }
        w.next();
    }
    dms.exec( "commit");
}

static void export_view(TermTablePtr table, const std::string& name, Sqlite dms) {
    std::string termname = name + "_term";
    std::string paramname = name + "_param";
    std::vector<String> props, tprops;
    for (Id i=0; i<table->params()->propCount(); i++) {
        const std::string& prop = table->params()->propName(i);
        if (prop!="id") props.push_back(prop);
    }
    for (Id i=0; i<table->termPropCount(); i++) {
        const std::string& prop = table->termPropName(i);
        if (prop!="param") tprops.push_back(prop);
    }
    std::stringstream ss;
    ss << "create view " << name << " as \n" << "  select ";
    for (Id i=0; i<table->atomCount(); i++) {
        ss << "p" << i;
        if (props.size() || tprops.size() || i!=table->atomCount()-1) {
            ss << ", ";
        }
    }
    ss << "\n";
    for (Id i=0; i<props.size(); i++) {
        ss << "\"" << props[i] << "\"";
        if (tprops.size() || i!=props.size()-1) ss << ", ";
    }
    for (Id i=0; i<tprops.size(); i++) {
        ss << "\"" << tprops[i] << "\"";
        if (i!=tprops.size()-1) ss << ", ";
    }
    ss << "  from " << paramname << "\n"
       << "  join " << termname << "\n"
       << "  on param=id";
    dms.exec(ss.str().c_str());
}

static void export_cts(System& sys, Sqlite dms) {
    /*
     * sqlite is case preserving but case insensitive, so that, e.g., if we hae
     * two ct fields which are the same when converted to lowercase, table
     * construction will fail with a "duplicate column" error.   To work around
     * this, detect names which would collide and give them an unlikely suffix
     * which we will remove on load.
     */

    std::map<std::string, std::string> cols;    // orioginal key -> converted key
    std::set<std::string> lowercase_cols;       // original key in lowercase
    Id salt = 0;
    for (Id id : sys.cts()) {
        for (auto& key : sys.ct(id).keys()) {
            std::string lowercase_key(key);
            std::string converted_key(key);
            to_lower(lowercase_key);
            if (lowercase_cols.find(lowercase_key) != lowercase_cols.end()) {
                converted_key = key + "_msys_converted_" + std::to_string(salt++);
            } else {
                lowercase_cols.insert(lowercase_key);
            }
            cols[key] = converted_key;
        }
    }
    std::string sql("create table msys_ct (id integer primary key, msys_name text,");
    for (auto& p : cols) {
        sql += "'" + p.second + "' text,";  // create table using the converted column name
    }
    sql.back()=')';
    dms.exec(sql.data());
    dms.exec("begin");
    auto w = dms.insert("msys_ct");
    for (Id id : sys.cts()) {
        auto& ct = sys.ct(id);
        w.bind_int(0, id);
        w.bind_str(1, ct.name());
        auto keys = sys.ct(id).keys();
        Id col = 2;
        for (auto& p: cols) {
            if (std::find(keys.begin(), keys.end(), p.first) == keys.end()) {
                w.bind_str(col, "");
            } else {
                write(ct.value(p.first), col, w);
            }
            col++;
        }
        w.next();
    }
    dms.exec("commit");
}


static void export_exclusion(TermTablePtr table, const IdList& map, Sqlite dms) {
    if (table->atomCount()!=2) {
        throw std::runtime_error("table with category exclusion has atomCount!=2");
    }
    dms.exec( "create table exclusion (p0 integer, p1 integer)");
    IdList ids = table->terms();
    Writer w = dms.insert("exclusion");
    dms.exec( "begin");
    for (Id i=0, n=ids.size(); i<n; i++) {
        IdList atoms = table->atoms(ids[i]);
        w.bind_int(0,map[atoms[0]]);
        w.bind_int(1,map[atoms[1]]);
        w.next();
    }
    dms.exec( "commit");
}

static void export_overrides( OverrideTablePtr o, std::string const& name,
                              Sqlite dms) {

    if (!o->count()) return;
    ParamTablePtr params = ParamTable::create();
    params->addProp("param1", IntType);
    params->addProp("param2", IntType);
    for (Id i=0; i<o->params()->propCount(); i++) {
        params->addProp(o->params()->propName(i), o->params()->propType(i));
    }
    std::vector<IdPair> olist = o->list();
    for (Id i=0; i<olist.size(); i++) {
        Id p = params->addParam();
        params->value(p,0) = olist[i].first;
        params->value(p,1) = olist[i].second;
        Id param = o->get(olist[i]);
        if (bad(param)) MSYS_FAIL("no param for override " << 
                olist[i].first << "," << olist[i].second);
        for (Id i=2; i<params->propCount(); i++) {
            params->value(p,i) = o->params()->value(param, i-2);
        }
    }
    export_params(params, name+"_combined_param", dms, false);
}

static void export_meta( TermTablePtr table, const std::string& name, 
        Sqlite dms) {
    std::string category(print(table->category));
    std::string sql("insert into ");
    sql += category;
    /* We really can't call the metatable for nonbonded "nonbonded_term". */
    if (category=="nonbonded") {
        dms.exec( "create table if not exists nonbonded_table (name text)");
        sql += "_table values ('";
    } else {
        sql += "_term values ('";
    }
    sql += name;
    sql += "')";
    dms.exec( sql.c_str());
}

static void export_nonbonded( TermTablePtr table, const IdList& map, Sqlite dms) {
    if (table->atomCount()!=1) {
        throw std::runtime_error("table with category nonbonded has atomCount!=1");
    }
    if (table->name()=="nonbonded") {
        export_params(table->params(), "nonbonded_param", dms);
        export_overrides(table->overrides(), table->name(), dms);
    } else if (table->name()=="alchemical_nonbonded") {
        /* skip, handled by export_alchemical_particles */
    } else {
        std::string const& name = table->name();
        export_terms(table, map, name+"_term", dms);
        export_params(table->params(), name+"_param", dms);
        export_view(table, name, dms);
        export_meta(table, name, dms);
    }
}

static void export_tables( const System& sys, const IdList& map, Sqlite dms) {
    dms.exec( "create table bond_term (name text)");
    dms.exec( "create table constraint_term (name text)");
    dms.exec( "create table virtual_term (name text)");
    dms.exec( "create table polar_term (name text)");
    std::vector<String> tables = sys.tableNames();
    for (unsigned i=0; i<tables.size(); i++) {
        const std::string& name = tables[i];
        TermTablePtr table = sys.table(name);
        if (!table->category) {
            std::stringstream ss;
            ss << "cannot export table '" << tables[i] << "' with no category";
            throw std::runtime_error(ss.str());

        } else if (table->category==EXCLUSION) {
            export_exclusion(table, map, dms);
        } else if (table->category==NONBONDED) {
            export_nonbonded(table, map, dms);
        } else {
            export_terms(table, map, name+"_term", dms);
            export_params(table->params(), name+"_param", dms);
            export_view(table, name, dms);
            export_meta(table, name, dms);
        }
    }
}

static void export_aux(const System& sys, Sqlite dms) {
    std::vector<String> extras = sys.auxTableNames();
    for (unsigned i=0; i<extras.size(); i++) {
        const std::string& name = extras[i];
        export_params(sys.auxTable(name), name, dms, false);
    }
}

static void export_nbinfo(const System& sys, Sqlite dms) {
    dms.exec(
            "create table nonbonded_info (\n"
            "  vdw_funct text,\n"
            "  vdw_rule text,\n"
            "  es_funct text)");
    Writer w = dms.insert("nonbonded_info");
    w.bind_str(0,sys.nonbonded_info.vdw_funct.c_str());
    w.bind_str(1,sys.nonbonded_info.vdw_rule.c_str());
    w.bind_str(2,sys.nonbonded_info.es_funct.c_str());
    w.next();
}

static void export_cell(const System& sys, Sqlite dms) {
    const GlobalCell& cell = sys.global_cell;
    dms.exec( 
            "create table global_cell (\n"
            "  id integer primary key,\n"
            "  x float, y float, z float)");
    Writer w = dms.insert("global_cell");
    dms.exec( "begin");
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            w.bind_flt(j+1, cell[i][j]);
        }
        w.next();
    }
    dms.exec( "commit");
}

static void export_provenance(System const& sys, Provenance const& provenance, 
                              Sqlite dms) {
    std::vector<Provenance> prov = sys.provenance();
    prov.push_back(provenance);
    dms.exec(
            "create table provenance (\n"
            "  id integer primary key,\n"
            "  version text,\n"
            "  timestamp text,\n"
            "  user text,\n"
            "  workdir text,\n"
            "  cmdline text,\n"
            "  executable text)");
    Writer w = dms.insert("provenance");
    dms.exec( "begin");
    for (unsigned i=0; i<prov.size(); i++) {
        w.bind_str( 1, prov[i].version.c_str());
        w.bind_str( 2, prov[i].timestamp.c_str());
        w.bind_str( 3, prov[i].user.c_str());
        w.bind_str( 4, prov[i].workdir.c_str());
        w.bind_str( 5, prov[i].cmdline.c_str());
        w.bind_str( 6, prov[i].executable.c_str());
        w.next();
    }
    dms.exec( "commit");
}

static void export_version(Sqlite dms) {
    dms.exec(
            "create table dms_version (\n"
            "  major integer not null,\n"
            "  minor integer not null)");
    Writer w = dms.insert("dms_version");
    w.bind_int(0,msys_major_version());
    w.bind_int(1,msys_minor_version());
    w.next();
}

/* make a mapping from id to 0-based dms primary key */
static IdList map_gids(System const& sys) {
    Id i,n = sys.maxAtomId();
    IdList ids(n, BadId);
    Id gid=0;
    for (i=0; i<n; i++) {
        if (!sys.hasAtom(i)) continue;
        ids.at(i)=gid++;
    }
    return ids;
}

static void export_dms(SystemPtr h, Sqlite dms, Provenance const& provenance,
                       unsigned flags) {
    System& sys = *h;
    IdList atomidmap = map_gids(sys);
    export_cts(      sys,            dms);
    export_cell(     sys,            dms);
    export_particles(sys, atomidmap, dms, flags & DMSExport::StructureOnly);
    export_bonds(    sys, atomidmap, dms);

    if (!(flags & DMSExport::StructureOnly)) {
        export_tables(   sys, atomidmap, dms);
        export_aux(      sys,            dms);
        export_nbinfo(   sys,            dms);
    }

    if (!provenance.version.empty()) {
        export_provenance(sys,provenance,dms);
    }
    export_version(                  dms);
    
    dms.finish();
}

class Tmpfile {
    char* _path = nullptr;
    char tmpl[32];

    char* mktemp() {
        unlink();
#if defined(_WIN64) || defined(_WIN32)
        strcpy(tmpl, "msys.pickle.XXXXXX");
        return ::_mktemp(tmpl);
#else
        strcpy(tmpl, "/tmp/msys.pickle.XXXXXX");
        _path = ::mktemp(tmpl);
        return _path;
    }
#endif

    void unlink() {
        if (!_path) return;
#if defined(_WIN64) || defined(_WIN32)
        ::_unlink(_path);
#else
        ::unlink(_path);
#endif
    }

    Tmpfile(Tmpfile const&) = delete;
    Tmpfile& operator=(Tmpfile const&) = delete;

public:
    Tmpfile() = default;
    const char* create() { return mktemp(); }
    ~Tmpfile() { unlink(); }
};

void desres::msys::ExportDMS(SystemPtr h, const std::string& path,
                             Provenance const& provenance,
                             unsigned flags ) {

    struct stat statbuf;
    if ((flags & DMSExport::Append) && 0==stat(path.data(), &statbuf)) {
        SystemPtr pre = ImportDMS(path);
        AppendSystem(pre, h);
        h = pre;
    }
    unlink(path.c_str());
    Sqlite dms;
    Tmpfile tmp;

    std::string _path;
    std::string compressed_ext = compression_extension(path);
    if (compressed_ext.size()) {
        _path = tmp.create();
    } else {
        _path = path;
    }

    try {
        dms = Sqlite::write(_path, flags & DMSExport::Unbuffered);
    } catch (std::exception& e) {
        MSYS_FAIL("Could not create dms file at " << path << ": " << e.what());
    }
    export_dms(h, dms, provenance, flags);
    if (compressed_ext.size()) {
        char buf[16384];
        std::ifstream ifile(_path);
        std::ofstream ofile(path);
        auto comp = compressed_ostream(ofile, compressed_ext);
        while (ifile.good()) {
            ifile.read(buf, sizeof(buf));
            if (ifile.gcount()) {
                comp->write(buf, ifile.gcount());
                if (!comp->good()) {
                    MSYS_FAIL("Error writing " << path);
                }
            }
        }
    }
        
}

static void no_close(sqlite3* db) {}

void desres::msys::sqlite::ExportDMS(SystemPtr h, sqlite3* db,
                                     Provenance const& provenance) {
    export_dms(h, std::shared_ptr<sqlite3>(db,no_close), provenance, 0);
}

std::string desres::msys::FormatDMS(SystemPtr sys, Provenance const& prov) {
    Tmpfile tmp;
    Sqlite dms = Sqlite::write(tmp.create());
    export_dms(sys, dms, prov, 0);
    return dms.contents();
}
