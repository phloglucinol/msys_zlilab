#ifndef desres_msys_system_hxx
#define desres_msys_system_hxx

#include <vector>
#include <map>
#include <cstddef>

#include "term_table.hxx"
#include "provenance.hxx"
#include "value.hxx"
#include "smallstring.hxx"

namespace desres { namespace msys {

    class GlobalCell {
        std::vector<double> data;

    public:
        template<class Archive>
        void serialize(Archive & archive) {
            archive(data);
        }

        GlobalCell() : data(9) {}
        double*       operator[](unsigned i)       { return &data.at(3*i); }
        const double* operator[](unsigned i) const { return &data.at(3*i); }


        /* Allow merge when corresponding vectors are equal, or when
         * self or other is identically 0, in which case the non-zero
         * cell is adopted. */
        void merge(GlobalCell const& other);
    };

    struct NonbondedInfo {
        String vdw_funct;
        String vdw_rule;
        String es_funct;

        template<class Archive>
        void serialize(Archive & archive) {
            archive(vdw_funct, vdw_rule, es_funct);
        }

        /* Allow merge when corresponding fields are equal, or when one
         * of them is empty, in which case the non-empty value is adopted.
         */
        void merge(NonbondedInfo const& other);
    };

    enum AtomType {
        AtomOther   = 0,
        AtomProBack = 1,
        AtomNucBack = 2,
        AtomProSide = 3
    };

    template <typename T>
    struct pod {    // Plain Old Data
        pod() { memset(this,0,sizeof(T)); }
    };

    struct atom_t : pod<atom_t> {
        Id  fragid;
        Id  residue;

        int8_t atomic_number;
        int8_t formal_charge;
        int8_t stereo_parity;
        int8_t aromatic;
    
        Float x,y,z;    /* position */
        Float charge;   /* partial charge */
        Float vx,vy,vz; /* velocity */
        Float mass;
    
        SmallString<30> name;
        AtomType type;

        template<class Archive>
        void serialize(Archive & archive) {
            archive(fragid, residue, atomic_number, formal_charge, stereo_parity, aromatic);
            archive(x,y,z,charge, vx,vy,vz,mass);
            archive(name, type);
        }
    
        /* Don't abuse these.  In particular, bear in mind that that an atom's
         * memory location will move around if atoms are added.  */
        double       *pos()       { return &x; }
        const double *pos() const { return &x; }
    };
    
    struct bond_t {
        Id  i=BadId;                /* id of first bond partner     */
        Id  j=BadId;                /* id of second bond partner    */
        int8_t order=1;             /* formal bond order            */
        int8_t stereo=0;            /* negative when atom order is flipped */
        int8_t aromatic=0;          /* no Kekule form specified     */

        template<class Archive>
        void serialize(Archive & archive) {
            archive(i, j, order, stereo, aromatic);
        }

        bond_t() {}
        bond_t(Id ai, Id aj) { i=ai; j=aj; }
        Id other(Id id) const { return id==i ? j : i; }
    };
    
    enum ResidueType {
        ResidueOther    = 0,
        ResidueProtein  = 1,
        ResidueNucleic  = 2,
        ResidueWater    = 3,
        ResidueLipid    = 4
    };

    struct residue_t {
        Id      chain;
        int     resid;
        SmallString<30> name;
        SmallString<6> insertion;
        ResidueType type;
    
        template<class Archive>
        void serialize(Archive & archive) {
            archive(chain, resid, name, insertion, type);
        }
        residue_t() : chain(BadId), resid(), type() {}
    };
    
    struct chain_t {
        Id      ct;
        String  name;
        String  segid;

        template<class Archive>
        void serialize(Archive & archive) {
            archive(ct, name, segid);
        }
    
        chain_t() : ct(BadId) {}
    };

    class component_t {
        ParamTablePtr _kv;

    public:
        template<class Archive>
        void serialize(Archive & archive) {
            archive(_kv);
        }

        /* constructor: maintain a single row with msys_name as the first
         * property. */
        component_t();

        /* must implement copy constructor so that we don't share _kv! */
        component_t(component_t const& c) { *this = c; }
        component_t& operator=(component_t const& c);

        /* getter/setter for name */
        String name() const;
        void setName(String const& s);

        /* other keys besides name */
        std::vector<String> keys() const;
        void del(String const& key);
        Id add(String const& key, ValueType type);
        ValueType type(String const& key) const;

        bool has(String const& key) const;
        ValueRef value(String const& key);
        ValueRef value(Id key);

        inline ParamTablePtr kv() { return _kv; }
    };

    class System : public std::enable_shared_from_this<System> {
    
        static IdList _empty;
    
        /* _atoms maps an id to an atom.  We almost never delete atoms, so
         * keep track of deleted atoms in a separate set.  This is needed only
         * by an atom iterator */
        typedef std::vector<atom_t> AtomList;
        AtomList    _atoms;
        IdSet       _deadatoms;

        /* additional properties for atoms */
        ParamTablePtr _atomprops;

        /* same deal for bonds */
        typedef std::vector<bond_t> BondList;
        BondList    _bonds;
        IdSet       _deadbonds;
        ParamTablePtr _bondprops;
    
        /* map from atom id to 0 or more bond ids.  We do keep this updated when
         * atoms or bonds are deleted */
        MultiIdList   _bondindex;
    
        typedef std::vector<residue_t> ResidueList;
        ResidueList _residues;
        IdSet       _deadresidues;
        MultiIdList   _residueatoms;  /* residue id -> atom ids */
    
        typedef std::vector<chain_t> ChainList;
        ChainList   _chains;
        IdSet       _deadchains;
        MultiIdList   _chainresidues; /* chain id -> residue id */
    
        typedef std::vector<component_t> CtList;
        CtList      _cts;
        IdSet       _deadcts;
        MultiIdList _ctchains; /* ct id -> chain id */
    
        typedef std::map<String,TermTablePtr> TableMap;
        TableMap    _tables;

        /* auxiliary tables.  basically a hack for cmap */
        typedef std::map<String, ParamTablePtr> AuxTableMap;
        AuxTableMap _auxtables;

        /* provenance.  Ideally, you would append to this just before 
         * serializing to disk. */
        std::vector<Provenance> _provenance;

    public:
        // Have to have an empty serialization func here because anything with a pointer to
        // System (like in TermTable) will need to believe that it can serialize this class
        // Actual serialization will happen with serialize_x() below.
        template<class Archive>
        void serialize(Archive & archive) { }

        // List of fields to serialize
        // There is probably some very clever way to do this with templates
        //
        //          WARNING          ACHTUNG         ADVERTENCIA       AVERTISSEMENT
        //
        //    When any new members are added to the System class, or to the classes contained
        //    as members underneath, the serialization version (CEREAL_VERSION in cereal.hxx)
        //    MUST be updated.
        //
        //    Any new members must have a serialize() method, and be added to the list below
        //    to ensure that they are properly serialized
        //
        //    The TestCereal unit test will catch things that change the HashSysem() return
        //    for serialized data vs. unserialized.
        //
#define _SER_LIST(_f)              \
                _f(name)           \
                _f(global_cell)    \
                _f(nonbonded_info) \
                _f(_atoms)         \
                _f(_deadatoms)     \
                _f(_atomprops)     \
                _f(_bonds)         \
                _f(_deadbonds)     \
                _f(_bondprops)     \
                _f(_bondindex)     \
                _f(_residues)      \
                _f(_deadresidues)  \
                _f(_residueatoms)  \
                _f(_chains)        \
                _f(_deadchains)    \
                _f(_chainresidues) \
                _f(_cts)           \
                _f(_deadcts)       \
                _f(_ctchains)      \
                _f(_tables)        \
                _f(_auxtables)     \
                _f(_provenance)

#define _SER_ENUM(_x)   S_ ## _x,
#define _SER_A(_x)      case S_ ## _x: a(_x); break;

        enum serialized_vars {
            _SER_LIST(_SER_ENUM)
            S_MAX
        };

        static inline size_t serialize_max() { return S_MAX; }

        // Serialize each piece. Can be done in parallel threads
        template <class Archive>
        void serialize_x(Archive &a, int index)
        {
            switch (index) {
                _SER_LIST(_SER_A)
                default: break;
            }
        }

#undef _SER_A
#undef _SER_ENUM
#undef _SER_LIST

        static std::shared_ptr<System> create();
        System();
        ~System();

        String          name;
        GlobalCell      global_cell;
        NonbondedInfo   nonbonded_info;

        /* get the provenance history */
        std::vector<Provenance>& provenance() { return _provenance; }
        std::vector<Provenance> const& provenance() const { return _provenance; }

        /* add a provenance entry.  Should be used only by ImportDMS! */
        void addProvenance(Provenance const& p) {
            _provenance.push_back(p);
        }

        /* element accessors */
        atom_t& atom(Id id) { return _atoms.at(id); }
        bond_t& bond(Id id) { return _bonds.at(id); }
        residue_t& residue(Id id) { return _residues.at(id); }
        chain_t& chain(Id id) { return _chains.at(id); }
        component_t& ct(Id id) { return _cts.at(id); }
    
        const atom_t& atom(Id id) const { return _atoms.at(id); }
        const bond_t& bond(Id id) const { return _bonds.at(id); }
        const residue_t& residue(Id id) const { return _residues.at(id); }
        const chain_t& chain(Id id) const { return _chains.at(id); }
        const component_t& ct(Id id) const { return _cts.at(id); }

        /* unchecked element accessors */
        atom_t& atomFAST(Id id) { return _atoms[id]; }
        bond_t& bondFAST(Id id) { return _bonds[id]; }
        residue_t& residueFAST(Id id) { return _residues[id]; }
        chain_t& chainFAST(Id id) { return _chains[id]; }
        component_t& ctFAST(Id id) { return _cts[id]; }

        const atom_t& atomFAST(Id id) const { return _atoms[id]; }
        const bond_t& bondFAST(Id id) const { return _bonds[id]; }
        const residue_t& residueFAST(Id id) const { return _residues[id]; }
        const chain_t& chainFAST(Id id) const { return _chains[id]; }
        const component_t& ctFAST(Id id) const { return _cts[id]; }

        /* id iterator, skipping deleted ids */
        class iterator {
            friend class System;
            Id            _i;
            const IdSet*  _dead;

            iterator(Id i, const IdSet* dead) 
            : _i(i), _dead(dead) {
                while (_dead && _dead->count(_i)) ++_i;
            }


            bool equal(iterator const& c) const { return _i==c._i; }
            const Id& dereference() const { return _i; }
            void increment() { do ++_i; while (_dead && _dead->count(_i)); }

        public:
            typedef std::forward_iterator_tag iterator_category;
            typedef Id value_type;
            typedef ptrdiff_t difference_type;
            typedef const Id* pointer;
            typedef const Id& reference;

            iterator() : _i(), _dead() {}
            const Id& operator*() const { return dereference(); }
            const Id* operator->() const { return &dereference(); }
            iterator& operator++() { increment(); return *this; }
            bool operator==(iterator const& c) const { return equal(c); }
            bool operator!=(iterator const& c) const { return !equal(c); }
        };

        template <typename T>
        void getPositions(T setter) const {
            for (iterator i=atomBegin(), e=atomEnd(); i!=e; ++i) {
                atom_t const& a = atomFAST(*i);
                *setter++ = a.x;
                *setter++ = a.y;
                *setter++ = a.z;
            }
        }

        template <typename T>
        void getVelocities(T setter) const {
            for (iterator i=atomBegin(), e=atomEnd(); i!=e; ++i) {
                atom_t const& a = atomFAST(*i);
                *setter++ = a.vx;
                *setter++ = a.vy;
                *setter++ = a.vz;
            }
        }

        template <typename T>
        void setPositions(T getter) {
            for (iterator i=atomBegin(), e=atomEnd(); i!=e; ++i) {
                atom_t& a = atomFAST(*i);
                a.x = *getter++;
                a.y = *getter++;
                a.z = *getter++;
            }
        }

        template <typename T>
        void setVelocities(T getter) {
            for (iterator i=atomBegin(), e=atomEnd(); i!=e; ++i) {
                atom_t& a = atomFAST(*i);
                a.vx = *getter++;
                a.vy = *getter++;
                a.vz = *getter++;
            }
        }

        /* iterators over element ids */
        iterator atomBegin() const { 
            return iterator(0, _deadatoms.empty() ? NULL : &_deadatoms); 
        }
        iterator atomEnd() const { return iterator(maxAtomId(), NULL); }
        iterator bondBegin() const { 
            return iterator(0, _deadbonds.empty() ? NULL : &_deadbonds); 
        }
        iterator bondEnd() const { return iterator(maxBondId(), NULL); }
        iterator residueBegin() const { 
            return iterator(0, _deadresidues.empty() ? NULL : &_deadresidues); 
        }
        iterator residueEnd() const { return iterator(maxResidueId(), NULL); }
        iterator chainBegin() const { 
            return iterator(0, _deadchains.empty() ? NULL : &_deadchains); 
        }
        iterator chainEnd() const { return iterator(maxChainId(), NULL); }

        /* reserve vector sizes */
        void atomReserve(size_t s) { _atoms.reserve(s); }
        void bondReserve(size_t s) { _bonds.reserve(s); }

        /* add an element */
        Id addAtom(Id residue);
        Id addBond(Id i, Id j);
        Id addResidue(Id chain);

        /* If ct is not supplied, add chain to first ct, creating a new one
         * if necessary */
        Id addChain(Id ct=BadId);
        Id addCt();

        /* delete an element */
        void delAtom(Id id);
        void delBond(Id id);
        void delResidue(Id id);
        void delChain(Id id);
        void delCt(Id id);

        /* One more than highest valid id */
        Id maxAtomId() const { return _atoms.size(); }
        Id maxBondId() const { return _bonds.size(); }
        Id maxResidueId() const { return _residues.size(); }
        Id maxChainId() const { return _chains.size(); }
        Id maxCtId() const { return _cts.size(); }

        /* list of elements ids */
        IdList atoms() const;
        IdList bonds() const;
        IdList residues() const;
        IdList chains() const;
        IdList cts() const;

        /* number of elements */
        Id atomCount() const { return _atoms.size() - _deadatoms.size(); }
        Id bondCount() const { return _bonds.size() - _deadbonds.size(); }
        Id residueCount() const {return _residues.size()-_deadresidues.size();}
        Id chainCount() const { return _chains.size() - _deadchains.size(); }
        Id ctCount() const { return _cts.size() - _deadcts.size(); }

        /* count of subelements */
        Id bondCountForAtom(Id id) const {
            if (id>=_bondindex.size()) return 0;
            return _bondindex[id].size();
        }
        Id atomCountForResidue(Id id) const {
            if (id>=_residueatoms.size()) return 0;
            return _residueatoms[id].size();
        }
        Id residueCountForChain(Id id) const {
            if (id>=_chainresidues.size()) return 0;
            return _chainresidues[id].size();
        }
        Id chainCountForCt(Id id) const {
            if (id>=_ctchains.size()) return 0;
            return _ctchains[id].size();
        }

        /* list of subelements */
        IdList const& bondsForAtom(Id id) const {
            if (id>=_bondindex.size()) return _empty;
            return _bondindex[id];
        }
        /* filtered list of subelements.  Predicate implements
         * bool operator()(bond_t const& b) const; */
        template <typename T>
        IdList filteredBondsForAtom(Id id, T const& predicate) const {
            IdList const& src = _bondindex.at(id);
            IdList dst;
            for (IdList::const_iterator it=src.begin(); it!=src.end(); ++it) {
                Id bid = *it;
                bond_t const& b = bondFAST(bid);
                if (predicate(b)) {
                    dst.push_back(bid);
                }
            }
            return dst;
        }

        IdList const& atomsForResidue(Id id) const {
            if (id>=_residueatoms.size()) return _empty;
            return _residueatoms[id];
        }
        IdList const& residuesForChain(Id id) const {
            if (id>=_chainresidues.size()) return _empty;
            return _chainresidues[id];
        }
        IdList const& chainsForCt(Id id) const {
            if (id>=_ctchains.size()) return _empty;
            return _ctchains[id];
        }

        IdList atomsForCt(Id id) const;
        Id atomCountForCt(Id id) const;
        IdList bondsForCt(Id id) const;
        Id bondCountForCt(Id id) const;

        IdList residuesForCt(Id id) const;


        /* is the the given element id valid? */
        bool hasAtom(Id id) const {
            return id<_atoms.size() && !_deadatoms.count(id);
        }
        bool hasBond( Id id ) const {
            return id<_bonds.size() && !_deadbonds.count(id);
        }
        bool hasResidue(Id id) const {
            return id<_residues.size() && !_deadresidues.count(id);
        }
        bool hasChain(Id id) const {
            return id<_chains.size() && !_deadchains.count(id);
        }
        bool hasCt(Id id) const {
            return id<_cts.size() && !_deadcts.count(id);
        }

        /* operations on term tables */
        std::vector<String> tableNames() const;
        /* fetch the table with the given name; return NULL if not present */
        TermTablePtr table(const String& name) const;
        /* get the name of the table; throw if table doesn't belong to this */
        String tableName(std::shared_ptr<TermTable const> table) const;
        /* rename the table with the given name; throw if no such table,
         * or if a table named newname already exists.  */
        void renameTable(String const& oldname, String const& newname);

        TermTablePtr addTable(const String& name, Id natoms,
                              ParamTablePtr ptr = ParamTablePtr() );
        void delTable(const String& name);
        void removeTable(TermTablePtr terms);

        /* invoke coalesce on each table */
        void coalesceTables();

        /* operations on auxiliary tables */
        std::vector<String> auxTableNames() const;
        ParamTablePtr auxTable(String const& name) const;
        void addAuxTable(String const& name, ParamTablePtr aux);
        void delAuxTable(String const& name);
        void removeAuxTable(ParamTablePtr aux);

        /* assign the atom to the given residue */
        void setResidue(Id atom, Id residue);

        /* assign the residue to the given chain */
        void setChain(Id residue, Id chain);

        /* assign the chain to the given ct */
        void setCt(Id chain, Id ct);

        /* extended atom properties */
        Id atomPropCount() const;
        String atomPropName(Id i) const;
        ValueType atomPropType(Id i) const;
        Id atomPropIndex(String const& name) const;
        Id addAtomProp(String const& name, ValueType type);
        void delAtomProp(Id index);
        ValueRef atomPropValue(Id term, Id index);
        ValueRef atomPropValue(Id term, String const& name);

        /* extended bond properties */
        Id bondPropCount() const;
        String bondPropName(Id i) const;
        ValueType bondPropType(Id i) const;
        Id bondPropIndex(String const& name) const;
        Id addBondProp(String const& name, ValueType type);
        void delBondProp(Id index);
        ValueRef bondPropValue(Id term, Id index);
        ValueRef bondPropValue(Id term, String const& name);

        /* find a bond between i and j (order independent), returning
         * its id or BadId if such a bond does not exist. */
        Id findBond( Id i, Id j) const;

        /* ids of atoms bonded to given atom */
        IdList bondedAtoms(Id id) const;

        /* bonded atoms satisfying a predicate.  predicate implements
         * bool operator()(atom_t const& atm) const; */
        template <typename T>
        IdList filteredBondedAtoms(Id id, T const& predicate) const {
            if (id>=_bondindex.size()) return _empty;
            IdList const& src = _bondindex[id];
            IdList dst;
            for (IdList::const_iterator it=src.begin(); it!=src.end(); ++it) {
                Id bid = *it;
                Id other = bond(bid).other(id);
                atom_t const& atm = atom(other);
                if (predicate(atm)) {
                    dst.push_back(other);
                }
            }
            return dst;
        }
    
        /* update the fragid of each atom according to its bond topology:
         * bonded atoms share the same fragid.  Return the number of
         * frags found, and atomid to fragment partitioning if requested */
        Id updateFragids(MultiIdList* fragments=NULL);

        /* Return ids of atoms based on their order of appearance in
         * a depth-first traversal of the structure hierarchy. */
        IdList orderedIds() const;

        inline ParamTablePtr atomProps() { return _atomprops; }
        inline ParamTablePtr bondProps() { return _bondprops; }
    };

    typedef std::shared_ptr<System> SystemPtr;

    int abi_version();

    void ReplaceTablesWithSortedTerms(SystemPtr mol);
}}

#endif
