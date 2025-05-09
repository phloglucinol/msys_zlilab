#include "graph.hxx"
#include "system.hxx"
#include "smiles.hxx"
#include "analyze.hxx"

using namespace desres::msys;

int main(int argc, char **argv){
    std::vector<std::string> smiles = {
        "C1CN2C3=C(C=CO3)C3=C2N(C1)C1=C3N=CO1",
        "CC1=C2C=CC(=O)C(OC2=C(N)N1)C#N",
        "C1NC2=C(N=C(S2)C2=CC=CS2)C2=C1NC=C2",
        "CC1C2=C(C=CC(C=O)=C1C)C(=NO2)C#N",
        "CC12CCOCC1C1=C(O2)C(F)=C(N)C(=O)N1",
        "CC1=C2C=CC(=O)C(OC2=C(N)N1)C#N",
        "CC1C2=CC(O)=CN2C2=C1NC(=O)C(N)=C2F",
        "CC12OC=NC3=C1C(=C(N)O3)C(C=O)=C2",
        "CC1=C2OCOC3=C(SC(=O)C=C3F)C2=CS1",
        "CC1=C2C3=C(NC=C3O)N3CC23CNC1=O",
        "CC1=C2SC=NC2=CSC(O)=CC=C(O)C1=N",
        "CC1=C2CNC(NC=CC(C)=C(N)C1=O)=C2O",
        "CC1=C2C=C(N)C=C2C(C#N)=C(N1)C(O)CN",
        "CC1=C2C(O)=C(SC2=CSC1=O)C1CCCN1",
        "CC1=C2C(SC=C3SC(=O)SC3=C2C)=CC1=O",
        "CC12NC3=C(N)OC4=C3C1=C(N4)C1=CON=C21",
        "CC1=C2C(C=O)N3C=C(O)C=C3C2=CNC1=O",
        "CC1=C2OCCC2=CC=C(NC=O)C(O)=CN1",
        "CC1=C2C(NC1=O)=C(NC(F)=C2O)C(N)CN",
        "CC1=C2C=C(N)C(O)=C2SC2=NC(=O)C=C2S1",
        "CC1=CC2=C(NC=C(NC(=O)CN)C=C2)C1=O",
        "CC1=C2C(=O)C3=C(SC=N3)C(O)=C2SC=N1",
        "CC1=C2NC(NC=C2)=CC=C(O)C(=O)C(C)=C1",
        "CC1=C2NC(N)=C(O)C2=C(F)N(CC#C)C1=O",
        "CC1=C2NC(NC=O)=CC(CO)=C2C=CC1=O",
        "CC1=C2C=NC(C3=CNC=C3N)=C2OC(O)=N1",
        "CC1=C2N(C=C(O)C3=C2C=CC(=O)N3)C(N)=C1",
        "CC1=CC2(CO)CNC3=C(O)C(C=O)=C1N23",
        "CC1=C2N(CC#N)C(=O)C=C2C(O)=C(CN)N1",
        "CC1=C2C=C(N3CC3)C(=O)C=C2C(O)=CN1",
        "CC1=C2SC=C(N)C=NC=C(O)C(S1)=CC2=O",
        "CC1=C2N=C3OCC3C3C=C(C=O)C(N1)=C23",
        "CC1=CC2=C(C=CNC=C(O)C=C2O)N1C=O",
        "CC1C2=C(NC=C2O)NC2=CC(=O)NC=C12",
        "CC1=C2SC(F)=C(O)SC2=C(C1=O)C(C)(C)C",
        "CC1=CC2=C(N1C1=C(C)NC=C1O)C(=O)CC2",
        "CC1=C2OCC(C)(CO)C3=CC(=O)SC(S1)=C23",
        "CC1CC2=C(C(O)=CS2)C(=O)C2=C1C(C)=CS2",
        "CC1=C(C)C=C2N=CC(C=C2O)=CC(N)=CO1",
        "CC1CC(C#C)N2C(=O)C(Br)=C(Br)C(O)=C2N1",
        "CC1=CC(C)=C(N)N(C=O)C2=C1C=C(N)C2=O",
        "CC1CC1N1C2=C(NC=C1O)C(=O)C=C2C",
        "CC1=C2C=CC=CSC=C(O)C(SC1=O)=C2O",
        "CC1=CC=C(C2=CC(N)=CN2)C(C)=CC=CN1",
        "CC1CC2=CC=C(O)C=C(NC=O)NC=C2O1",
        "CC1=C2OC(OC=N)=CC(C)=C2C(N)=C1C#N",
        "CC1=CC2=C(C1=O)C(C)=C1NC(O)=CC1=CN2",
        "CC1=C2NC=C(O)C=CC=CN2C(=O)C(N)=C1",
        "CC1=CC2=C(C=CNC=C(O)NC=C2C)C1=O",
        "CC1(CC2CCN1C2)C(=O)C1=NC(=NO1)C#N",
        "CC1=C2C3=C(S1)C(=O)SC=C3OCC2(O)C#N",
        "CC1=CC2=CN3C=CC(O)=C3CCN2C1=O",
        "CC1=C2C3=C(SC=C3OC2(CO)C=O)SC1=O",
        "CC1=CC2=C(N)C(=CC2=O)C=CNC=C(O)N1",
        "CC1=C2CC3=C(OC2=NO1)C1=C(NC(O)=C1)N3",
        "CC1=C(CNC1)C1=C(N)C(=O)C(O)=CN1",
        "CC1=CC2=C(N1)C(=O)C(O)=CNC(N)=C(N)N2",
        "CC1CCN2C=C(O)C(=C12)C1=CC(=O)C=CN1",
        "CC1=CC2=C(SC3=C2OC(C)(C)CC3)SC1=O",
        "CC1=CC=NC(CN)=C(N)C2=C(NC=C2O)O1",
        "CC1CCCC2=C(N1)C(=O)NC1=C2C(O)=CN1",
        "CC1CC1C1=C(O)C2=C(S1)SC(=O)C(F)=C2",
        "CC1=CNC2=C1C1=C(NC2=O)C(C)(CN)CO1",
        "CC1=C(F)C(=O)N2C(=C1)C(N)C1=C2NC=C1O",
        "CC1=CC2=C(NC(O)=CC=CN1)C(=O)C=C2N",
        "CC1=C2C=C(SC1=O)C(O)=CSC=CC=C2O",
        "CC1=CC2=C(O1)C=CNC=CC1=C2NC(=O)N1",
        "CC1=CC(=O)NC=CC=CC2=CNC(CO2)=C1",
        "CC1=C2C(NC1=O)=C(NC=C2O)C1CNC=N1",
        "CC1=C2C(NC1=O)=C(C)N(CC#C)C(N)=C2O",
        "CC1=CC=C(N)C=NC=C(O1)C1=CNC=C1O",
        "CC1=C2C(N=CC=C2C)=C(O1)C1=CNC=C1O",
        "CC1=C(C)C(O)=C(O)C(C)=C(N)N(C=O)C=C1",
        "CC1(CCC2=C(O1)C1=C(NC(C=O)=C1)N2)C#C",
        "CC1=C2C(=O)C3=C(SN=C3)C(O)=C2SC=N1",
        "CC1=C(C)C(O)=CN1C1=C(Br)C(=O)NC(Br)=C1",
        "CC1=C2C(O)=C(N)NC2=C(N)N(CC#C)C1=O",
        "CC1=CNC2=C1C1=C(OC=C1)N=CC=C(O)O2",
        "CC1=C(C=CC=C(CO)NC=C1)C1=CC=CN1",
        "CC1=C(NC=C1)C1=C(O)N(C(N)=O)C(=C1)C#N",
        "CC1=CC2=C(NC=C2O)NCCNC1=O",
        "CC1=C2NC(N)=C(OC(C)(C)C)C2=CNC1=O",
        "CC1(C2OC1C=C2)C(=O)C1=NOC(=N1)C#N",
        "CC1CC2N3C(N)=C(O)C(C=O)=C3C=C2C1",
        "CC1=C(O)C=C(CO)N1C1=C(C)NC(=O)C=C1",
        "CC1=C2NC(F)=C(NC2=C(C)C1=O)OCC#C",
        "CC1=CC(N)=C(O)SC=NC2=CC(=O)C=C2S1",
        "CC1=CC=CNC(=O)C(O)=CNC=C(N)C=C1",
        "CC1=CC=CC(=N)C=C(O)C2=C(OC(O)=N2)O1",
        "CC1=C(C=O)C2=C(S1)SC(O2)=NC1=CC=CN1",
        "CC1=C(C=C(CN)N1)C1=C(F)C(=O)C(O)=CN1",
        "CC1=CC=CC2=C(OC=C2)C(C)=C(NC=O)N1",
        "CC1=CN(C(=C1)C(=O)CO)C1=CNC(N)=C1O",
        "CC1CCCC(C1C)C1=NC(=NO1)C(=O)C#N",
        "CC1=C(F)NC(=O)C2=C1C(O)=C(CNC=N)N2",
        "CC1=C(O)C2=C(CC(O)CO)SC(=O)C=C2S1",
        "CC1=C(OCC#N)C2=C(N1)C(=O)NC(C)=C2N",
        "CC1=C(N)C2=C(NC(O)=C(N2)C(F)(F)F)C1=O",
        "CC1=CC(O)=CC2=CC(O)=C(NC=O)N12",
        "CC1COCC1(C)CC1=NOC(=N1)C(=O)C#N",
        "CC1=CC(O)=C(NC2=C(N)C(F)=CSC2=O)S1",
        "CC1=C(CO)C(O)=CNC(N)=CC=C1C=O",
        "CC1=C(NC=O)C=C2C1=COC(N)=C2C#N",
        "CC1=C(C(N)=O)C(O)=C(CN)SC(=N)SC=C1",
        "CC1=C(C)C(OC=N)=COC=C(C#N)C(N)=C1",
        "CC1=C(O)C(=CS1)C1=C(Cl)C(=O)SC(C)=C1Cl",
        "CC1=C(N)C2=C(N)C=CN(C=O)C2=C(N)C1=O",
        "CC1=C(N)C(O)=CC(=N)C=C2OCOC2=CO1",
        "CC1=C(CC2CC2)C2=C(N1)C(=O)NC=C2O",
        "CC1=CC(CN)=C2C=C(F)C(O)=C(NC=O)N12",
        "CC1C(OC(=N)C1=O)C1=NC(=NO1)C(=O)C#N",
        "CC1=CN2C(NC(N)=O)=CC(N)=C2C(O)=C1O",
        "CC1=CC(C#N)=C2C(O)C(=O)NC2=COC=C1",
        "CC1=CNC=CC(=O)C2=C1NC(N)=C(O)C=C2",
        "CC1=CSC(N)=C(O)C=C(O)C2=C1NC(=O)S2",
        "CC1=C(NC2=C1NC=C(O)C2=O)N1CCCC1",
        "CC1=C(N)C(O)=C(N1)N=C1SC(=O)SC=C1F",
        "CC1=C(O)C(F)=C(N)C(=O)N1CCCCC#C",
        "CC1=CC2=C(C(O)=C(N)N2)C(=O)C(C)(C)C1",
        "CC1(C)CNC(C2=CNC=C2O)=C1C=O",
        "CC1=CC(=O)C2=CC(O)=CN2C(N)=CNC=C1",
        "CC1=C(OC(N)=C1)C1=CC=COC(N)=CC=C1",
        "CC1=CC(=O)N2C3=C(OCC2=C1C)C(N)=CN3",
        "CC1CN2C=CC=CC(=O)C(N)=CC(O)=C2C1",
        "CC1CC2=C(N1)C1=C(N2C)C(=O)C(O)=C(F)N1",
        "CC1=CC2=C(N1)SC(=O)SC1=C(OC(O)=N1)S2",
        "CC1=NC(=O)C2=C(S1)C(OC(C#N)C#N)=NS2",
        "CC1=CC2=CC(N)=C(C)OC=CC(=NO)C1=C2",
        "CC1=C(O)C=CC(=N)C=C2OCCNC2=CO1",
        "CC1=C(N)NC=C2C(O)=C(NC(N)=O)C(O)=C12",
        "CC1=C(C=O)C(N)=C(N)C(O)=CNC=C1CO",
        "CC1=C(N)C(=O)C2=C(SC=C(N)N1)C(O)=CS2",
        "CC1CC2=C(O1)C(=O)C1=C(N2)C(=CC1)C#N",
        "CC1=CN(C=O)C2=CC(=O)C=CC2=C1N",
        "CC1CC2=C(O1)C1=C(S2)C(=O)SC(C)=C1",
        "CC1=C(O)C2=C(N1)C(=O)N1CC(=NO)C1C=C2",
        "CC1=C(C=O)C(OC1)=NC1=CNC=C1CO",
        "CC1=C(C#N)C2=C(O1)C=CSC(=N)SC(=O)S2",
        "CC1=CC2=C(O)C(=CN=COC(N)=C1)C(O)=C2",
        "CC1=C(O)N=C(O)C(=O)C=C2SC=NC=C2S1",
        "CC1=CC(O)=C2C3CC(=O)C4=CC(N)=C(N34)N12",
        "CC1=CC(=O)C=CNC2=C(O)C(NC2)=C1C#N",
        "CC1=C(O)C=C(N1CC=O)N1C=CC=C1C=O",
        "CC1=C(F)C(O)=C(N)N(C(CO)CCN)C1=O",
        "CC1=CC(=O)C2=C1N(CC(N)CO)C(O)=CN2",
        "CC1=CC2=C(O)C(S1)=CC(=O)C=C(N)SC=N2",
        "CC1=CC(O)=C(O)C(N)=C(O)C(N)=CN1C=O",
        "CC1COC2=C(C1)NC1=C(N(C)C=C1)C2=O",
        "CC1=CC2=C(S1)C1=CSC(C)=C(O)C=C1C2=O",
        "CC1=C(O)C(F)=C(N)C(=O)N1CC1COCO1",
        "CC1=C(O)OC=C2NC(=O)CC2=C(C=C1)C#N",
        "CC1=C(N)C2=C(N1)C(=O)C(O)=CNC(N)=CN2",
        "CC1CC2=C(SC(=O)SC(C)=NC=C2)N=CO1",
        "CC1=CC(O)=CN2C(=O)NCC2=C(C)C(O)=C1",
        "CC1=C(F)C(O)=C2NCC(CC=O)CN2C1=O",
        "CC1=CC(=O)NCC(C)(C)C2=C1C(O)=CN2",
        "CC1=CNC(=O)CC2=C1NC1=C2C=CC1=O",
        "CC1=CNC2=C1N=C(O2)C1=CCC(O)C1=O",
        "CC1=CNC(N2C(Br)=CC(Br)=C(Br)C2=O)=C1O",
        "CC1=CSC(=O)C(C)=C2N=CSN=C(O)C2=C1",
        "CC1=CC(=O)C=C(N)C=C2NCCC(N2)=C1O",
        "CC1=C(NC=O)C(C)=C2C=C(O)C(O)=C(F)N12",
        "CC1=CC(O)=CN2CC=CC2=CC(=O)N1",
        "CC1OCC2=C(O1)C1=C(NC(C=O)=C1)N2C",
        "CC1=NC=C2OCC(=N)OC2=C(N)SC(=O)S1",
        "CC1C(=O)C=C(C2=C(N)NC=C2O)C1(C)O",
        "CC1=CN(C2=CNC(CN)=C2O)C(=O)C(F)=C1",
        "CC1C(=O)N(C)C2=C1C=CC(O)=CC(O)=CN2",
        "CC1=C(N)N2C=CC(O)=C(NC(N)=O)C2=C1O",
        "CC1=NC2=C(NC(CN)=C2O)SC(=O)SC=C1",
        "CC1C(N)C1C1=C(O)C2=C(S1)C(=O)SC=C2",
        "CC1=C(O)C2=C(N1)N1C(C2)=CC2=C1C(=O)NC2",
        "CC1=C(O)C=C(N1CC#N)C1=CC=CNC1=O",
        "CC1COC2=C(SC3=C2C(=O)SC=C3F)C1N",
        "CC1=CN(C(CO)=C1O)C1=CNC(=O)C=C1N",
        "CC1N2C=C(C#N)C(C#CC#C)=C2CC1=O",
        "CC1=CN(CC#N)C=C(O)C(N)=CC(C=O)=C1",
        "CC1=CN(C=O)C(N)=CC(O)=C(O)C(C)=C1C",
        "CCC1=C(O)C2=C(C)C(=O)NC(CC#C)=C2N1",
        "CC1=C(O)NC=C1C1=CN=CC=COC(C)=N1",
        "CCC1=C(NC(=O)C=C1N)N1C=C(C)C(O)=C1",
        "CC1=C(O)C2=C(S1)C(=O)C=C(S2)C1CCNC1",
        "CC1=C(C=CC=C(NC=O)OC(N)=C1O)C#N",
        "CC1=CC=C(C#N)C2=C(O)C(F)=C(C)C(=O)N12",
        "CC1CNC2=C1N=CSC(=O)SC1=C2OC=N1",
        "CC1(N)COCC2=C1C1=C(N2)C(=O)NC=C1O",
        "CC1=C(OC(S1)=NC1=CC=C(N)S1)C(=O)CO",
        "CC1=C(O)C2=CC(=O)C=C2SC=CC=C(O)S1",
        "CC1=CC=C(C=C(O)C=CN1)C1=C(N)C=CO1",
        "CC1=C(N)C(O)=CC(=N)C(O)=C(O)C(O)=CO1",
        "CC1COCCC(C1)C(=O)C1=NOC(=N1)C#N",
        "CC1=CNC(=O)C=C1N1C=C(O)C=C1CCO",
        "CC1=C(O)C(=NC2=CSC(=O)S2)C(=CS1)C#N",
        "CC1=C(O)C(O)=C(N)NC(N)=CC(NC=O)=C1",
        "CC1=CC=CN2C=C(O)C=CC2=C(N)NC1=O",
        "CC1=COC(=C1)C1=NC(N)=C(C)OC=CC=C1",
        "CC1=C(O)C(=C(C)N1)C1=CC(=O)C(Br)=C(Br)N1",
        "CC1=C(N)N2C(=C1)C(=O)C=C(CN)C(O)=C2C",
        "CC1=CSC(=O)C2=C1SC(COC=N)=C2O",
        "CC1=C(NC=O)C(O)=CC2=C(O)C(CN)=CN12",
        "CC1=C(O)C2=C(CCC3=C2SC=CC3=O)S1",
        "CC1=C(O)C2=C(N1)C(=O)SC=NC=C(S2)C#N",
        "CC1N2C(C=O)=C(N)C(C)=C2N2C=C(O)C=C12",
        "CC1=CNC(N)=C2OCCC2=CC(=O)C(C)=C1",
        "CC1=C(O)C(F)=C(C(N)=O)C2=CC(O)=C(N)N12",
        "CCC1=C(C)C2=C(NC(CC#C)=C2O)C(=O)N1",
        "CC1NC2=C(OCC1(C)O)C(Br)=C(Br)C(=O)N2",
        "CC1=C(O)C2=C(C)N(C(N)=O)C(N)=C(O)C2=C1",
        "CC1OC2=CNC(C(=O)N3CC3)=C2C=C1",
        "CC1(C)CCNC2=C1C1=C(NC=C1O)NC2=O",
        "CC1=CNC(N)=C(O)C(C)=C2CNC(=O)C2=C1",
        "CCC1=C(F)C(F)=C(N2C=CC(O)=C2)C(=O)N1",
        "CC1=C(N)C(NC=O)=C2N1C=C(O)C(F)=C2C",
        "CC1=CC=C(O)NC=CC2=C(N)C(=O)C=C2N1",
        "CC1=C(N)NC=C1OC1=CNC(=O)C(C)=C1F",
        "CC1=C(CN)C2=C(O)NC(NC=O)=CC2=C1O",
        "CC1=C(O)C=CN1C1=C(F)C(F)=C(C)C(=O)N1",
        "CC1=C(O)C(=O)C2=CC=C(C)SC=CC2=CS1",
        "CCC1=CC(=O)N2C(F)=CC=C(O)C2=C1C#N",
        "CCC1=CC2=C(N1)OC(O)=C(C)C=NC(O)=C2",
        "CC1=C(O)C(NC2=C(N)NC(=O)C=C2)=C(N)N1",
        "CC1NCC2C3=C(OC12C)C(F)=C(N)C(=O)N3",
        "CCC1=CC(N)=C(C)C(O)=CC(O)=CN1C=O",
        "CC1=COC=C(C#N)C(O)=C(O)C2=C1N=CO2",
        "CCC1CC2=C(NC3=C2C(O)=CN3)C1=O",
        "CCC1=CC(O)=NC=CC2=C(NC(CN)=C2)O1",
        "CC1=CC(N)=C(O)NC2=C(NC=C1)C(=O)C=C2",
        "CC1=CSC(=O)C2=C(SC=NC=C2O)N=C1C",
        "CC1=NC(O)=C2C(S1)=C(O)C1=C2SC(=N)C=C1",
        "CC1OCC(O)C2=C1C1=C(N2)C(=O)NC=C1O",
        "CCCC1=C(CO)C(=O)C2=C1NC(O)=C(F)N2",
        "CCC1=NC2=NOC(=N)N=C2C(=N1)C(=O)C#N",
        "CC1CC(=O)C2=CC(N)=CNC=C(O)C=C2N1",
        "CC1NCC(CC2=NOC(=N2)C(=O)C#N)C1O",
        "CCC1=C(CC#N)C=C2C1=C(C)NC=C2C#N",
        "CC1OCC2OC3=C(NC4=C3C=C(N4)C=O)C12",
        "CCCC1=C(CN)C(=O)C2=C1NC(O)=CN2C",
        "CCC1=CC(C)=C(C=CC=CS1)C1=CC=CS1",
        "CCC1(OC2=CNC(=O)C3=C2C1=CN3)C(C)C",
        "CCC1=CNC(CO)=C(O)C=C(N)C(C=O)=C1",
        "CCC(C)NC1=C(O)NC2=C(N)C(=O)C=C2N1",
        "CC1N2C=C(O)C(N)=C2C2=CNC(=O)C(F)=C12",
        "CC1=CC(O)=C(O)C(C)=C(N)NC(NC=O)=C1",
        "CC(C)C1=C2C(O)=C(CN)NC2=C(F)C(=O)N1",
        "CC(C)(C)C1=C2SC(=O)SC=C2C(O)=C1O",
        "CC(C)C1=C(C(C)OC2=CNC=C12)C(N)=O",
        "CC1(O)C(OC2=C1SC1=C2SC=CC1=O)C=O",
        "CC1=C(CO)NC=C(O)C=C(NC=O)C=C1O",
        "CC1NC2=C(O)C(F)=C(CCN)C(=O)N2C1C",
        "CCC1=C(N2C=CC=C2C=O)C(OC)=C(C)N1",
        "CC(C)(C)CNC1=C(O)NC2=CC(=O)C=C2N1",
        "CCC1CCC=C(C=O)C2=C1ON=C2C#N",
        "CC1OCOCC2=C1C1=C(NC=C1O)NC2=O",
        "CC1=C(NC2=C1C1=C(N2)C2(CO2)CO1)C=O",
        "CCC1=CC(=O)C=C2N(C(N)=O)C(F)=CC=C12",
        "CC1=CC(=O)NC2=C1C(O)=CN2C1CNC1",
        "CC1=CNC2=C1CC1=C(O)C=C(CO)N1C2=O",
        "CCC(C)CCC(O)C1=NOC(=N1)C(=O)C#N",
        "CCCNC1=C(O)C2=C(C=C(F)NC2=O)N1C",
        "CC1=CNC2=C1C(O)=C(C#CC#C)N2C=O",
        "CC(C)C1=CNC(NC=O)=CC(O)=C(O)C=C1",
        "CC1NCC1C1=CSC2=C(SC=C2O)C1=O",
        "CC1=C(NC2=CNC(=O)C=C2)NC(CN)=C1O",
        "CC(C)(CC(O)C=O)C1=NC(=NO1)C(=O)C#N",
        "CC1=C(F)C(O)=C(C#N)N(CCC(O)=O)C1=O",
        "CC1(OC(=N)C2=C1C(=O)SC(=N)S2)S(N)(=O)=O",
        "CCC1=C2C(OC3=C(N)NC=C23)=CN(C)C1=O",
        "CC1=CNC2=C(C=CC2=O)C=C1NC=O",
        "CC1=CNC=C(C=C2C=CC(=C2)C=C1N)C#N",
        "CCCC1=NC2=CN(C=C2O1)C1=CSC(=O)S1",
        "CC1=CNC=C1C1=C2N=CC(O)=C(O)C2=CO1",
        "CC1=NOC(=N)C2=C(S1)C(=O)C=CC(O)=CS2",
        "CCN1C=C(O)C(=O)C2=C1C1=C(C2)NC(N)=C1",
        "CCC1CC2=C(C1)C1=C(NC=C1OC)C(=O)N2",
        "CC1=C(N)C=CC(=O)C=CC(OCC#C)=CN1",
        "CC(C#C)C(C)N1C=C(O)NC2=CC(=O)C=C12",
        "CC1=C(N)C=C(N1)C1=CC=CN2C=C(N)C=C12",
        "CCC1=C2C(=CC1=N)C(O)=C1OC=NOC=C21",
        "CC1=C(N)C=CN(C2=C(O)C(=CN2)C#N)C1=O",
        "CC1=CN(C=O)C(N)=CC(O)=CC(O)=C1CN",
        "CC1=C(N)NC=C(O)C2=C(C=CN2)C(=O)C=C1",
        "CC1=NSC(CC(=O)C2=NC(=NO2)C#N)=C1N",
        "CC1=C(N)C(OC2=NSC(=O)C(C)=C2F)=CS1",
        "CC1=C(NC(N)=C(O)C1=O)C1=C(N)NC(N)=C1",
        "CCC1=C(O)C2=C(S1)SC(=O)N=C2C#N",
        "CCC(C)OC1=CC(O)=C2NC(=O)N(C)C=C12",
        "CC1=C(N)C(O)=C(N)C(O)=CN(C=C1)C(N)=O",
        "CC1=CN=CSC=C(O)SC2=C(N)C(=O)C1=C2",
        "CC1=C(NC=O)C2=C(NC=CC(C)=C2N)C1=O",
        "CC1=NC(O)=CC2=C(NC=C2)OC=C1CCN",
        "CCC(C)(O)C1=C(O)C2=CC(=O)NC2=C(N)N1",
        "CC1=C(N)C(=O)C=CNC(O)=C2C=CC(N1)=C2",
        "CCCOC1=CNC2=C1C(=O)N(C)C(N)=C2F",
        "CCC1=C(N)C2=CN(C=O)C=C2C(O)=C1OC",
        "CCN1C(=O)C2=C(NC(N)=C2OC)C=C1CO",
        "CC1OC=NC2=COC(N)=C2C2=CNC=C2O1",
        "CC1=C(N)C(=O)C=C(N)C(CO)=CNC=C1O",
        "CC1(CO)C2=C(O)C=CN2C2=CNC(C=O)=C12",
        "CCC(=O)NC1=C(O)C(F)=CC2=C(O)C=CN12",
        "CCCCC1=C(O)C2=C(S1)C=C(CN)SC2=O",
        "CCC1=C(F)N(C(C)=O)C(O)=C2C=C(N)C=C12",
        "CCC1CN1CC1=NOC(=N1)C(=O)C#N",
        "CCC1=C(O)C2=C(SC(=N)S2)C(=O)N1C",
        "CCC1(C)COC=NC2=C1NC1=C2SC(=O)S1",
        "CCC1=C(O)C=C(O)C2=C(SC(=O)S2)C=CS1",
        "CC1=C(O)C=C(O)C2=C(CNC(=O)N2)C=CN1",
        "CCC1=C(O)C(C2=C(C)SC=C2)=C(C)C(=O)S1",
        "CC(C)C1=C(NC=O)NC(O)=C2C=C(C)C=C12",
        "CCC(=O)CN1C2=C(NC=C1O)C(=O)C=C2C",
        "CCC(O)COC1=C(F)NC=C2NC(=O)C=C12",
        "CCC(CNC1=C(F)C(O)=CNC1=O)C(C)O",
        "CC1=C(O)C=NC(=O)C(=N)SC(C#N)=C(C)S1",
        "CCC1=CC2=C(CC#C)NC(N)=C(C#N)C2=C1",
        "CC1=C(O)C(=O)C2=CC=CN2C2=CC(O)=CN12",
        "CC1(C)OCC(=O)C2=C1C1=C(NC(N)=C1O)N2",
        "CCC1=C2C=CC=C2C(C#N)=C(CC#N)N1",
        "CCN1C=C(O)C(=CC1=O)C1=C(N)N(C)C=C1",
        "CC1=CSC(O)=C2C=CC(=C2)C=C(O)SC1=O",
        "CC(=N)OC1=C(NC=C1)OC1=C(N)SC(=O)S1",
        "CC1=C(O)C(O)=C2N1C=C(N)C(F)=C2NC=O",
        "CCC1=C(C(NC)=CN1)C1=CC(=O)C(O)=CN1",
        "CC1=C(O)C(O)=C(NC=O)NC=CC(O)=C1N",
        "CCN1C=C(OC)C2=C1C(C)C(=C2)C(N)=O",
        "CC(C#C)N1C(C)=CC2=C(N(C)C=C2O)C1=O",
        "CC(C)C(CC(=O)C1=NOC(=N1)C#N)C(N)=N",
        "CC1N2C(=CC1=O)C=CN1C2=CC(O)=C1C",
        "CCC1=CC(O)=C(N=C2SC(=O)SC=N2)N1C",
        "CCOC1=CNC2=C(CN=CN)C(=O)NC=C12",
        "CCC(C(=O)C1=NC(=NO1)C#N)C(C)(C)NC",
        "CCN1C=C2N(C(C)C)C(N)=C(O)C2=CC1=O",
        "CCNC1=CC(C)=CN(C=O)C=CC(O)=C1O",
        "CC(C)C1=C2N(C=O)C=CC(N)=C2C=CC1=O",
        "CCOC1=CSC(CO)=C1C1=CC=CSC1=O",
        "CCCOC1=C(N)SC(=O)C2=C1C(N)=CS2",
        "CCC(=O)NC1=C(O)C(N)=CNC=CC(O)=C1",
        "CCCOC1=C(CN)NC2=CC(=O)C(C)=C2N1",
        "CC1=C(O)NC2=C1SC1=C(OC=N1)SC(=O)S2",
        "CC(CCO)C1=C2NC=C(O)C2=C(C)C(=O)N1",
        "CCCOC1=C2N(C)C(=O)NC=C2C(OC)=C1",
        "CCC1=C(O)C2=C(NC=C(N)C=CC(=O)S2)S1",
        "CCC1=C(O)C(C2=CNC=C2N)=C(N1)C(N)=O",
        "CC1NC2=CC1=C(N)C(=O)C(C)=CC(O)=CN2",
        "CC(C(CO)N(C)C)C(=O)C1=NOC(=N1)C#N",
        "CCNC1=CC2=C(NC=O)C=CC2=C(N1)OC",
        "CC(C)(O)C1=C(O)C2=C(N1)C(=O)NC=C2",
        "CCCC1=COC=C(NC)C(O)=CC(=N)C=C1",
        "CC1C(=O)OCC2=C1C1=C(SC=C1O)C(=O)S2",
        "CC(OC=O)C1=C(O)N(C)C2=C(N1)C(=O)C=C2",
        "CC(NC=N)C1=C2C(O)=C(C)SC2=CC(=O)S1",
        "CCC1=CC(=O)C(CN)=CC(O)=CNC=C1N",
        "CC(C)(C=O)N=COC1=NOC(=N1)C(=O)C#N",
        "CCCNC1=C(F)C(O)=C(F)N(CC#N)C1=O",
        "CC(C)N1C(=O)C(C(O)CO)=C(F)C(O)=C1N",
        "CCN=C1SC2=C(C(=O)S1)C(C)(OC2=N)C=O",
        "CC(C)CC1=C(O)C2=C(C)C(=O)NC2=C(N)N1",
        "CCCC1=C(OC)C2=C(NC(N)=C2C)C(=O)N1",
        "CCCN=C1ON=C(CC#N)N=C1C(=O)C#N",
        "CCCC1=C2NC(C)=C(O)C2=C(N)C(=O)N1C",
        "CC(N(C)CC1=NOC(=N1)C(=O)C#N)C(O)=O",
        "CCCC(CC=O)NC1=C(O)C(F)=CC(=O)N1",
        "CC1=NC=C2C=C(Cl)C(=CSC(=N)S1)C(Cl)=C2",
        "CC(C)C1=CC=C(C)OC=C(N)C2=C1C=NO2",
        "CCC1=C(O)C(=NO)C(F)=C(S1)C1=CSC=N1",
        "CCC1=C(N)C(=O)C(N)=CC=C(C)NC=C1O",
        "CC(C)(CN)CNC1=C(O)C(F)=C(F)C(=O)N1",
        "CCC(=O)CC1=C(O)C2=C(N1)C(=O)NC(N)=C2",
        "CC(N)C(C)(C)N1C=C2C(O)=CNC2=CC1=O",
        "CCC1=CC(=O)N(C=C1C)C1=C(N)C(O)=CN1",
        "CCC1=CN=CC(O)=COC2=C1C(OC)=CN2",
        "CN1C(C)=C2NC(=O)C=C2C(O)=C1COC=N",
        "CCOC1=C(CO)OC2=C(C=CN2)C=CN=C1",
        "CC(O)CN1CCOC2=C1NC(=O)C(Cl)=C2Cl",
        "CC(=O)C1=C(N2C=C(N)C(O)=C2N)C(N)=CN1",
        "CCCC1=CNC=C1C1=C(C)C(=O)NC=C1O",
        "CC(=N)NC(=O)CC1=C(F)C(O)=C(N)NC1=O",
        "CC(CCO)COC1=CSC2=CSC(=O)C=C12",
        "CCN(C)C1=C2C(O)=C(F)NC=C2C=CC1=O",
        "CC(C)C1=C2N(C=C1)C(=O)N1C=C(O)C(O)=C21",
        "CCOC(C#C)C1=NC(=NOC(=N)C1=O)C#N",
        "CCC1=C(O)NC2=C(N1)C(=O)C=C2C1CO1",
        "CC(C(=O)C=O)C1=C(O)C2=CC(=O)NC2=CN1",
        "CC(C#C)N1C=C(O)C(=C1)C1=C(C=O)C=CN1",
        "CC(=O)C1CC2=C(O1)C1=C(S2)C(=O)SC=C1F",
        "CC(C)(CN)C1=C(O)C2=C(NC(N)=C2)C(=O)N1",
        "CCOC1=CNC(=O)C2=C1C(C)=C(CO)N2C",
        "CC(CO)NC1=C2C=CC(=O)C=C2C(O)=CN1",
        "CC(C)(CN=CN)N1C=C(O)C(F)=C(N)C1=O",
        "CCN1C(CN)=C(OC)C2=C1N(C)C(=O)C=C2",
        "CCOC1=C(CN)N(C)C2=CC(=O)NC(F)=C12",
        "CC(O)COC1=CNC2=C1C(C)=CC(=O)N2C",
        "CC(C)C(O)C1=CC=C2C1=CNC(N)=C2C#N",
        "CC(OC=O)C1=CC2=C(N1)C(=O)C(O)=CN2C",
        "CC(=O)C1=C(N)C2=C(NC(CO)=C2O)N1",
        "CN1C=C2OCC3=CN(CC(C)=O)C(=C23)C1=O",
        "CCC(C#N)N1C(=O)C(CC)=C(F)C(O)=C1N",
        "CCC1(O)COC2=C1NC1=C2C(=O)NC=C1N",
        "CCOC1=CNC(NC2=CC=C(N2)C=O)=C1C",
        "CCOC1=CSC(=N)SC=CC(NC)=CC1=O",
        "CC(CCN)N1C2=C(C=CC2=O)N(C)C=C1O",
        "CC(C)(C)OCC(=O)C1=NC(=NOC1=N)C#N",
        "CCC(NC)C(C#N)C(=O)C1=NOC(=N1)C#N",
        "CCCC1=CC(=O)C2=C1NC=C(O)N2CC",
        "CN1C2=CC(=O)C=C2N(CC=O)C(C#N)=C1O",
        "CCN1C=CC2=C1C(=O)N(CC#N)C=C2OC",
        "CC(C)C(N)C1=C(C)C2=C(C(O)=CN2)C(=O)N1",
        "CC(C)COC(=N)CC1=NOC(=N1)C(=O)C#N",
        "CCC(C)(N)CCN1C(=O)C=C(F)C(O)=C1N",
        "CCOC1=C(C)SC(SC=NC=C1C=O)=NC",
        "CC(NC=N)C(=N)OC1=NOC(=N1)C(=O)C#N",
        "CC(CC1=C2N(C)C(=O)C=C2C(O)=CN1)C#N",
        "CN1C=C2OCC(=C2C)C(=O)C=C(N)C=C1C",
        "CC(=O)N1C=C2C(O)=C(N)C(O)=C2C(Br)=C1Br",
        "CCCN(C=O)C1=CC2=C(O)NC=CC2=C1C",
        "CN1C2=C(N)C(=O)C(N)=C2NC=C1O",
        "CCNC1=C(OCC#N)C2=C(N1)NC(=O)C=C2",
        "CC(N)C1NC2=C(OC1C)C(Cl)=C(Cl)C(=O)N2",
        "CC(C)CC(N)CC1=C(F)C(O)=C(N)NC1=O",
        "CCCN1C=C(OC)C2=C(NC(C)=C2C)C1=O",
        "CCC(C)(O)COC1=C(C)NC(=O)C(N)=C1F",
        "CN1C=C2C(=O)C=C(F)C(O)=C2C(N)=C1CO",
        "CC(C)OC1=C(C)NC2=C1C=CNC2=O",
        "CC(C#N)C1=C(O)C2=C(N)C(=O)NC2=C(F)N1",
        "CCC(OC1=C(N)NC(=O)C(Br)=C1Br)C(C)O",
        "CC(CN)COC1=CNC2=CC(=O)NC(N)=C12",
        "CC1OCC2=C(O1)C1=C(SC=C1CN)C(=O)S2",
        "CC1OCCNC2=C(O1)C(F)=C(CO)C(=O)N2",
        "CC(O)C1=C(O)SC2=C(C)C(=O)C(CO)=C2S1",
        "CN1C=CC2=C1N=C(O)C(O)=COC(N)=C2C",
        "CCCC1=C(C(O)=CS1)C1=CC(=O)C(F)=CS1",
        "CCC(CC)(CC)N1C(=O)C=C(F)C(O)=C1N",
        "CC(C)(O)C1=C2OC(=N)CN2C2=C1SC(=O)S2",
        "CCN1C(C=O)=CC2=C1N1C=C(O)C=C1C2C",
        "CC(C)OC1=CSC(=NC(N)=CSC1=O)C#N",
        "CC(CO)OC1=CN(C)C2=C(N)C(=O)C=C2N1",
        "CC(CO)N1C(C)=C(O)C2=C(C)NC(=O)C=C12",
        "CC(C)C(C)CC1=C2SC(=O)C=C2C(O)=CS1",
        "CCCN1C2=C(O)C(C)=C(OC)C2=CNC1=O",
        "CN1C=CC2=CC=C(O)N2C2=C1C(=O)C=C2N",
        "CCC(C)(C#C)C1=C(O)C(F)=C(N)C(=O)N1C",
        "CCN1C=C2C(O)=C(N)NC2=C(NC)C1=O",
        "CCN1C2=CC(=O)NC(C)=C2C(O)=C1C(C)N",
        "CC(=O)N1C(NCCO)=CC2=CC=CC2=C1O",
        "CCN1CC(OC2=C1NC(=O)C(F)=C2F)C#N",
        "CCN1C2=C(C=C(C)C1=O)C(OC(C)C)=CN2",
        "CC(CCC#C)C1=CSC(=O)C2=C1C(O)=CS2",
        "CCN1C2=C(NC(F)=C1O)C(=O)C(=C2)C(C)N",
        "CCN1C=C(O)C2=C1N=C(C)C=CSC(=O)S2",
        "CCNC1=C(O)OC=C(C)C=CC(=N)C=C1O",
        "CCC(C)N1C=C(OC)C2=C1C(C)=CNC2=O",
        "CCC1=C(C2=CSC(C)=C2O)C(=O)SC(F)=C1",
        "CCN1C=C2C(O)=C(CC#N)NC2=C(F)C1=O",
        "CCCNC1=COC=CC(C)=CC(=N)C=C1O",
        "CC(=O)N1C=C(O)C(N)=C(O)C(N)=CC=C1C",
        "CCC(N)C1=C(C)C2=C(S1)C(=O)SC(F)=C2O",
        "CCN1C2=CC(=O)C=C2N(C=C1OC)C(C)C",
        "CC(N)C1=C(CCN)C2=C(S1)C(=O)SC=C2O",
        "CC(N)C1=C(O)C2=C(S1)SC(=O)C(=C2)C(C)N",
        "CCC1=CC2=C(NC1=O)N(CCO)C(C)=C2O",
        "CCNC1=CN=CSC2=C(NC=C2O)C(=O)S1",
        "CCNC1=CC(=O)C2=C(S1)C(OC)=C(N)S2",
        "CN1C=CC2=C1OC=C(O)C=NC(O)=C2",
        "CC(CN)N(C)C1=CC2=C(NC=C2O)NC1=O",
        "CCC1=CC2=C(F)N(C)C(NC)=C(C#N)C2=C1",
        "CCN(C)C1=C2COC3=C(C)NC(=C23)C(=O)N1",
        "CC(CN)OCCC1=C(O)C(F)=C(N)C(=O)N1",
        "CN1C2=C(C(O)=CN2)C2=C(NC(CO)=C12)C=O",
        "CC(=N)NCC1=C(F)C(=O)C2=C(S1)C(O)=CS2",
        "CCC(CO)N1C(=O)C(NC)=C(F)C(O)=C1C",
        "CCC1=CC=C(NC=O)NC2=CC(=O)C(N)=C12",
        "CCN(CC1=C(O)C2=CSC(=O)C=C2S1)C=O",
        "CC(N=C1ON=C(O)C(=O)C(=N1)C#N)C(N)=N",
        "CC(C(=O)C1=NC(=NO1)C#N)C(C)(C)NC=O",
        "CC(N)(CO)C1=C2SC=C(O)SC2=CC1=O",
        "CCCNC1=CC2=CNC(C)=C(O)C2=C1C=O",
        "CCOC1=C2NC(C)CN2C2=C1C=CC(=O)N2",
        "CC(=N)OC1=COC=C(C#N)C(N)=CC(O)=C1",
        "CCNC1=CC(O)=CC=CC(O)=CN1C(N)=O",
        "CCC1=CC(=O)C2=CC(=CC(O)=C(N)N12)C#N",
        "CCCN(C)C1=C(O)C(=O)C2=C(N1)C=CN2C",
        "CCC1=CC(=C(N)O1)C1=C(NC=C1)N=COC",
        "CC(=O)NC1=CC2=C(C=C(N)C2=C(O)O1)C#N",
        "CCC1=CC(N)=C(NC=O)C(O)=CNC=C1O",
        "CCC1=C(CO)C2=C(N1C)C(=O)C(O)=CN2",
        "CC(C#N)C(=O)C1=C(N)C2=C(NC(N)=C2O)N1",
        "CN1C2=C3C(NCCOC3=C(C)N2)=C1C=O",
        "CC(C#N)N1C=C(O)C2=C(Br)C(=O)NC(Br)=C12",
        "CN1C=C2CC(C)(CN)N3C=C(O)C(=O)C1=C23",
        "CCC1=C(C=O)N2C=C(N)C3=C2C1OC=N3",
        "CCC1=C(C)OC2=C(C=C(O)N2C)C=CN=C1",
        "CCC(O)C1=CC2=C(N(CC)C(O)=CN2)C1=O",
        "CCC1=CC=C(N1)N1C=C(O)C(F)=C(C)C1=O",
        "CCC1=C(CO)C=C2N1C1=C(O)C=CN1C2=O",
        "CN1CC2=CC(=O)C=C(CN)C=CNC1=C2O",
        "CCC1=C(N)C2=C(C)N(C)C=C(O)C=C2C1=O",
        "CCC1=CN(C=O)C=C(O)C(O)=CC(N)=C1",
        "CCN1C(=O)C(NCCCO)=C(F)C(O)=C1F",
        "CC(N)C(N(C)C)C1=C(F)C(O)=C(N)NC1=O",
        "CC(C1=C(NC2=C1C(O)=CN2)C=O)C(C)(C)O",
        "CC(OC(C)(C)C=O)C1=NOC(=N1)C(=O)C#N",
        "CCC1=NC2=C(O1)C=CN=C(CC)SC(=O)S2",
        "CCOC1=C(CC=O)NC(=O)C2=C1C=CN2C",
        "CCOC1=CN(C=C1C)C1=CC(N)=CNC1=O",
        "CC(O)C1=C2C=C(O)C=CN2C(NC=O)=C1O",
        "CCC1=CSC(O)=C(N)C2=C(C=NS2)C(O)=C1",
        "CCNC1=CC2=C(N)C(O)=CC2=C(O)N1C=O",
        "CC(=N)OC1=C(N)C=C(N)N1C1=CSC(=O)S1",
        "CCC1=C(O)C2=C(N1CC=O)C(=O)NC(F)=C2",
        "CC(CN)(NC1=C(O)C(Cl)=C(Cl)C(=O)N1)C#C",
        "CCOC1=C(Cl)C(=N)C(=O)C2=C1SN=C(Cl)S2",
        "CCOC1=CNC=C2NC(=O)C(C(N)CO)=C12",
        "CCOC1=CNC(=O)C(NCC(N)C#N)=C1F",
        "CCNC1=C(F)C2=C(NC1=O)C(O)C(N)CO2",
        "CCC1CNCCN2C(=O)C(N)=C(F)C(O)=C12",
        "CCNC1=C2N(C=O)C=CC=CC2=C(C)C1=O",
        "CCOC1=C(F)N(C)C=C2C=CC(=O)C(N)=C12",
        "CC(=O)N1C=C(C)C=C(O)C(N)=C(O)C=C1N",
        "CCC(=O)C(O)C1=C2C(O)=CNC2=CNC1=O",
        "CC(=O)CC(OC1=C(N)NC(=O)C=C1F)C#N",
        "CCOC1=CNC(=C1)N1C(=O)C=CC(N)=C1F",
        "CC(C)OC1=C(C)N(C)C2=C1C=C(N)C(=O)N2",
        "CCC(=O)OCC1=C2C(O)=CSC2=CSC1=O",
        "CC(=O)C1=C(N)C(NC2=CNC(N)=C2O)=CN1",
        "CC(O)C1=C(C(O)=CN1)C1=CC=C(N)NC1=O",
        "CC(O)(CN)CN1C(=O)C(N)=C(F)C(O)=C1N",
        "CC(N)CCOC1=CNC(=O)C2=C1C=CN2C",
        "CC(O)CCC1=C2C(C)=CC=C2C(=CN1)C#N",
        "CC(=O)NC1=CNC(C)=C2OCOC2=CC=C1",
        "CCN1C=C(O)C2=C1C=C(CC#N)N(C)C2=O",
        "CC(N)C1=CC2=C(NC(CN)=C(O)N2C)C1=O",
        "CN1C=C2C=C(N)C(=O)C=CN2C=CC(O)=C1",
        "CN1CC2=C(O)C=CC(=O)C=C(C)C(N)=C1N2",
        "CC(C)C1=C(C)C(=O)C2=C(S1)C1=C(CCO1)S2",
        "CN1C=C2C(O)=CN(C3CC3)C2=CC1=O",
        "CC(O)C(=O)CNC1=C(F)C(O)=CN(C)C1=O",
        "CC(=O)NC1=C(C=C2C=C(Br)OC(Br)=C12)C#N",
        "CN1C2=C(NC(F)=C1O)C(=O)C=C2COC=O",
        "CC(=O)NC1=C(N)C(=O)C2=C1C=CC(C)=CN2",
        "CC(N)CN1C=C(O)C2=CC(=O)NC(CN)=C12",
        "CN1C=CC2=CC(=O)C=CNC=C(O)C2=C1F",
        "CN1CC2=C(O)C1=CC(=O)C(N)=C(N)C=CN2",
        "CCN(CC)C1=C(OC(C)C)C(F)=CC(=O)N1",
        "CN1C=CC2=C1OC=NCC1CC=C2C1=O",
        "CCCC1=C(O)C2=C(N1)N(CC#N)C(=O)C=C2",
        "CCCC1=C(OC)C(=O)C2=C(C=C(C)N2)N1C",
        "CC(=O)C1=C(C=CN1)N1C(C)=C(C)C(O)=C1N",
        "CC(N)CNC1=C(N)SC(=O)C2=C1SC=C2O",
        "CC(C)(C)C1=C2N(C=C1O)C1=C(NC=C1)C2=O",
        "CC(N)CNC1=C(N)C=CN=C(O)SC(=O)S1",
        "CCOC1=C(CC)NC2=C(N1)C(=O)C(CN)=C2",
        "CCCC1=C(O)C2=C(NC=C2COC)C(=O)N1",
        "CC(=N)OCC(O)C1=C(O)C(F)=C(N)C(=O)N1",
        "CCN(C(N)=O)C1=C(O)C=CC=CNC=C1O",
        "CCNCC1=C(C)C2=C(N1)C(=O)C(O)=C(C)N2",
        "CC(OC1=CNC=C2NC(=O)C=C12)S(O)(=O)=O",
        "CN1C=C2OC(C(O)C#N)C3=C(NC1=C23)C=O",
        "CCNC(=N)CN1C(=O)C(N)=C(F)C(O)=C1C",
        "CCC(C)C1=C(CO)C2=C(NC=C2O)C(=O)N1",
        "CCOC1=CC2=C3SC(=O)SC=C3C=CC2=C1",
        "CCC(C)(CC1=NC(=NO1)C(=O)C#N)OC=O",
        "CC(C#C)C1=CC(=CC(=O)S1)C1=CSC=C1O",
        "CCOC1=CNC(N2C=C(F)C=CC2=O)=C1C",
        "CC(OC1=NC=NN1)C1=NOC(=N1)C(=O)C#N",
        "CCOC=NCC1=C(O)C2=CC(=O)SC2=CS1",
        "CC(=O)N1C(N)=C(O)C=CC=C(O)C(N)=C1N",
        "CC(=O)NC1=C(C)C=CNC2=C(C=CO2)C=C1",
        "CCC(C)CC1=C2C(SC1=O)=C(F)SC=C2O",
        "CC(=O)NC1=C(O)C(O)=CC=CNC(C)=C1C",
        "CC(C)(C)CNCC1=NOC(=N1)C(=O)C#N",
        "CCC(C)C1=C(O)NC2=C(N1)C(N)=C(N)C2=O",
        "CC(O)CC(C)(C)C1=NC(=NO1)C(=O)C#N",
        "CCCC(C)OC1=C(C)NC2=CNC(=O)C=C12",
        "CC(OC1=C2SN=C(C#N)C(=O)C2=CS1)C#C",
        "CCC(CC#N)(OC)C1=NC(=NO1)C(=O)C#N",
        "CC(C)CC(CC=O)C(=O)C1=NC(=NO1)C#N",
        "CN1C(C)=C2C3=C(N=CO3)C(N)C3=COC1=C23",
        "CCC(C)COC1=C(N)N(CC)C(=O)C=C1F",
        "CC(C)CN=C1SC2=C(NC(N)=C2O)C(=O)S1",
        "CCOC=NC1=C(O)C=C(N)SC(=O)C=CS1",
        "CN1CC2=C(O)C=C(C)C(=O)C=C(C)C=C1N2",
        "CN1C2=C(C=C(C)C2=O)C=C(C)C=C1NC=O",
        "CC(CCO)(OC1=NC(=NO1)C(=O)C#N)C#C",
        "CCC(CO)OC1=NOC(=N)C(=NC1=O)C#N",
        "CCC(CN)OC1=CNC2=C1C(N)=CC(=O)N2",
        "CN1C=C2OCC3=C(NC4=C3C2=C1N4)C(N)=O",
        "CC(C)(C)OC1=CNC2=C1C(CN)=CC(=O)N2",
        "CC(C)N1C=C(O)NC2=CC(=O)C(C)=C12",
        "CC(CN)C1OC2=CN(C)C3=C2C1=CC(=O)N3",
        "CN1C=CC2=C1C1=C(COC(=N)O2)SC(=O)S1",
        "CCC(=O)C1=C(N)C(O)=CSC(=N)SC(C)=C1",
        "CC(C)(O)C1=C(O)C2=C(N)NC(=O)C(F)=C2N1",
        "CC(C)(OCC1=C(F)C(O)=C(N)NC1=O)C#N",
        "CCCOC1=CSC2=C1SC=C(OCC)C2=O",
        "CC(C)NCC(C)OC1=CNC(=O)C(N)=C1F",
        "CC(C)OCC1=C(O)NC2=CC(=O)C(N)=C2N1",
        "CCC(OCC1=C(F)C(O)=C(N)NC1=O)C=O",
        "CCC(OC)C1=C2C(O)=CNC2=CC(=O)N1",
        "CC(C)OC1=CSC2=C1SC(=CC2=O)C(C)O",
        "CCN1C2=C(N(C)C(O)=C1N)C(=O)C(N)=C2",
        "CC(CO)OC1=C(C)N(C)C2=C1C=C(N2)C=O",
        "CC(N)C1=CC2=C(NC=C2OCCO)NC1=O",
        "CCC(=O)CC1=C(O)NC2=C(N1)C(=O)C(C)=C2",
        "CCNC1=C2N=COC2=CN1C1=CSC(=O)S1",
        "CCN1CC2CN3C=C(O)C4=C3N2C(=C4)C1=O",
        "CCN1C(=O)C2=C(C=C(C)N2)C(O)=C1CC=O",
        "CCNC1=C(O)C=CC(=CC=CN1)C(=O)NC",
        "CCN1C(=O)C(N)=C(F)C2=C1CC(N)CCO2",
        "CCN1CC(C)COC2=C1NC(=O)C(C)=C2F",
        "CCN1C=CC(C2=CN(C)C=C2O)=C(F)C1=O",
        "CC(N=C1ON=C(N=CC1=O)C#N)C(F)(F)F",
        "CCNC1=C(N)C2=C(C)NC=C(O)C=C2C1=O",
        "CCNC1=CN2C(C=C(O)C(F)=C2C=O)=C1N",
        "CC(N)C1=CSC(C)=C(O)C(=O)N=C(S1)C#N",
        "CCN=C1SC(=O)SC=C1C1=C(N)NC=C1O",
        "CC(N)C(C)N1C=C(N)C2=C1C(=O)C(O)=CN2",
        "CC(N)COC1=C(N)SC2=CC(=O)C(N)=C2S1",
        "CC(=O)C1=CC2(C)N=COC3=C2N1C=C3",
        "CCOC1=C(C)NC2=C1C(N)=CN(CC)C2=O",
        "CC(OC1=CNC2=C(N(C)C=C2C)C1=O)C#C",
        "CCOC1=CNC(=O)C2=C1C(=CN2)C(O)CC",
        "CCOC1=C(N)C2=CN(C=O)C(C)=C2C=C1O",
        "CC(O)C1=CC=C(C=C(N)C(O)=CN1)C(N)=O",
        "CC(O)C1=CC(N)=C2C1=CN(C=O)C(C)=C2O",
        "CC(=O)C1=CCN2C1=CC1=C2CCOC=N1",
        "CCOCC1=C(O)C2=C(SC=C2)C(=O)S1",
        "CC(O)C1=C(N)C2=C(N)NC=C(O)C=C2C1=O",
        "CCOC(C)CC1=CC2=C(NC(O)=CN2)C1=O",
        "CC(O)CC(O)C1=C(O)NC2=C(N1)C=CC2=O",
        "CCOC(CO)=NCC(=O)C1=NOC(=N1)C#N",
        "CCOC1=NC=C(C)C(O)=C(NC)SC(=O)S1",
        "CC(O)CN1C=CC2=C1C(=O)NC(C)=C2O",
        "CC(O)(C#N)C1=C(NC2=C1SC(=O)S2)OC=N",
        "CC(=O)N1C(N)=CC(O)=CC=C(N)C(O)=C1N",
        "CC(=O)NC1=C(NC(C)=C1)C1=CC(=CO1)C#N",
        "CC(=O)NC1=CNC=C(O)C(O)=C(C)C=C1C",
        "CCOCCN(C)C1=C(O)C(Br)=C(Br)C(=O)N1",
        "CC(OCC#N)=NC1=C(C)NC2=C1SC(=O)S2",
        "CN1C=CC2=C1C(=O)CNCC1=C2C(O)=CN1",
        "CN1C2=C(CC(=O)N2)C(O)=C1C1=CC(O)=CN1",
        "CN1C=CC2=C1N=COCC1=C(N2)SC(=O)S1",
        "CN1C=C2OC(C#N)C3=CNC=C3C2=C1C=O",
        "CN1C=C2OCC3=C(C2=C1)C(=O)NC(N)=C3N",
        "c1ccccc1",
        "c1ccc2ccccc2c1",
        "O=CC=C[O-]",
        "[NH2+]=CC=CN",
        "[O-]P([O-])([O-])=O",
        "c1ccc([N+](=O)[O-])nn1",
        "C[NH+]=C(N)N",
        "[NH2+]=C(N)N",
        "Cn1c[n+](C)c2cncnc21",
        "CC(=O)C=[N+]=[N-]",
        "P(c1ccccc1)(c1ccccc1)c1ccccc1",
        "O=C1c2cccc3cccc(c23)C(=O)N1",
        "Brc1ccc(-c2c3nc(c(-c4ccc(-c5ccncc5)cc4)c4ccc([nH]4)c(-c4ccc(Br)cc4)c4nc(c(-c5ccc(-c6ccncc6)cc5)c5ccc2[nH]5)C=C4)C=C3)cc1",
        "Brc1cccc(-c2nc3ccccc3c(-c3ccccc3)c2Oc2ccc(-c3cc(-c4ccccc4)c4ccccc4n3)cc2)c1",
        "c1[nH]c(N)c2c1C=CC(=O)C(C#N)O2",
        "BrC1=C2CCC(N2C=CC1=O)C1=NSC(=N)N1",
        "CC1=NN=C(Cl)C1=C[O-]",
        "CC1=NC(=S(C)(=O)[O-])N=C1c1ccccc1",
        "c1ccccc1Cc2ccccc2Cc3ccccc3"
    };

    for (const auto& s : smiles) {
        SystemPtr system = FromSmilesString(s);
        AssignBondOrderAndFormalCharge(system, AssignBondOrder::ComputeResonantCharges);
    }

    printf("OK\n");
    return 0;
}
