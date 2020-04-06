/*************************************************************************************************
Detector construction class is to define the geometry and materials of the detector you are using.
The world is where the detector is placed and it is a box with vacuum. The NNS detector is then
placed at the center of the world volume. NNS detector is made with concentric cylinders(Tubs) 
of HDPE material and a gold foil at the center of the cyllinders. The dimensions of the shells 
are taken from the actual measurements performed on the NNS. The Tubs are hollow cyllinders and 
hence to make the NNS shells with lid and bottom,a union of different solids are made for each shell 
configuration seperately. 
The user can fix the number of shells needed in a simulation using command line arguments. 
If the user does not specify the number, by default it is set to zero. This corresponds to the 
bare gold foil with no HDPE shells around. The gold foil material is a mixture of both gold and 
aluminum. The radius of the foil is set as that of the inner most NNS shell. Thickness is obtained
from the data sheet given with the gold foil. 
*************************************************************************************************/

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"


#include "G4Orb.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4AutoDelete.hh"


#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

G4double goldfoil_density = 0;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(),
   fGoldArea(0),
   fGoldMass(0),
   fShellnumber(0),
   fScorerNumber(0),
   fCheckOverlaps(true)
   { }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


DetectorConstruction::~DetectorConstruction()
   { }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4VPhysicalVolume* DetectorConstruction::Construct()
   {
    DefineMaterials(); // call the define material function.
  
    return DefineVolumes();   // Define volumes
   }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



/************************************************************************************
All the materials used in the detector construction are defined in the DefineMaterial
function. For materials defined using the nistManager, the interaction cross sections
are taken from the NIST data base. Elements defined using G4Element method takes the
corresponding interaction cross sections from the updated Geant4 database.
*************************************************************************************/
void DetectorConstruction::DefineMaterials()
   { 
    auto nistManager = G4NistManager::Instance();  // Calling NIST Manager
    
    nistManager->FindOrBuildMaterial("G4_Galactic"); // Vacuum as defined in NIST
      
    // =============================================
    // Constructing gold foil material.
    // Note that the Aluminium is referred as TS_Aluminium to include the thermal 
    //  neutron scattering cross sections from the updated database.
    G4Element* el_Al = new G4Element("TS_Aluminium_Metal","Al",13.,26.98*g/mole);
    G4Element* el_Au = new G4Element("Gold", "Au", 79, 196.97*g/mole);
    
    goldfoil_density         = 19.3*g/cm3 ; //obtained from the specification sheet
    G4Material* Gold_mat =  new G4Material("Gold_mat",goldfoil_density,2);
    Gold_mat-> AddElement(el_Au, 99.99*perCent); // Note that we use pure gold foil
    Gold_mat-> AddElement(el_Al, 0.01*perCent); // No information about the 0.01% trace metals. Al was an arbitary choice.

    //================================================
    //constructing the Polyethylene
    // NNS material - High density polyethylene (HDPE)
    // Includes the thermal scattering of H
    // Composition is taken from NIST data and replaced it with thermal Hydrogen.
    G4Element* el_C = new G4Element("C","C",6,12.0107*g/mole);
    G4Element* el_H = new G4Element("TS_H_of_Polyethylene","H",1.,1.0079*g/mole);

    G4Material* Polythene = new G4Material("Polythene",0.945*g/cm3,2,kStateSolid,293.15*kelvin);
    Polythene->AddElement(el_H,0.143711);
    Polythene->AddElement(el_C,0.856289);

    //================================================
    //constructing the Polyoxymethylene (Acetal)
    // Its for the new insert to keep the gold in place.
    // Includes the thermal scattering of H (assumed its similar to polythene since a seperate thermal hydrogen was not available)
    // Composition is taken from NIST data and replaced it with thermal Hydrogen.
    G4Element* el_O = new G4Element("O","O",8,15.999*g/mole);

    G4Material* Polyoxymethylene = new G4Material("Polyoxymethylene",1.42*g/cm3,3,kStateSolid,293.15*kelvin);
    Polyoxymethylene->AddElement(el_H,2);
    Polyoxymethylene->AddElement(el_C,1);
    Polyoxymethylene->AddElement(el_O,1);
   }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



/***************************************************************************************
The detector volume is defined in the DefineVolume function. There are three volumes defined.
The World volume, NNS volume and Gold volume. Each of them are made up with a solid volume, 
logic volume and a physical volume. Solid volume has the information about the geometry, 
logic volume set its properties and the physical volume sets its physical parameters like 
its position, alignment etc.
****************************************************************************************/
G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
   {
    // Get materials 
    auto vacuum   = G4Material::GetMaterial("G4_Galactic");
    auto foil_mat = G4Material::GetMaterial("Gold_mat");
    auto NNS_mat  = G4Material::GetMaterial("Polythene");
    auto Acetal   = G4Material::GetMaterial("Polyoxymethylene");
    
    //==========================================================================
    //1. World volume
    //==========================================================================
    G4Orb* SolidWorld = new G4Orb("World",200*cm); //full solid sphere of radius 200 cm.

    G4LogicalVolume* LogicWorld
       = new G4LogicalVolume(SolidWorld,    // its solid
                             vacuum,        // its material
                            "World");       // its name
    fWorldVolume = new G4PVPlacement(0,                // no rotation
                           G4ThreeVector(),  // at (0,0,0)
                           LogicWorld,       // its logical volume                         
                           "World",          // its name
                           0,                // its mother  volume
                           false,            // no boolean operation
                           0,                // copy number
                           fCheckOverlaps);  // checking overlaps 
                                 
                                
    //==========================================================================
    // 2. NNS volume
    //==========================================================================
    //The new insert made of Acetal to keep gold in place.
    G4double gold_thick = 0.01*cm; //real value is 0.01*cm
    G4double gold_rad   = 0.8*cm; // this is the inner radius of the first shell and of goldfoil
    G4double small_insert_len = 2.54*cm;   // length of the small part of the whole insert
    G4double big_insert_len = 5.46*cm;  // length of the big part of the insert
    
    // The small insert is used while performing the measurement with bare foil
    G4Tubs* small_insert = new G4Tubs("small_insert",0.0,gold_rad-0.001,small_insert_len/2,0,360);  //length needs to be given as half length. For some weired reason, tube radius shld be smaller than gold for the visualization.
    if (fScorerNumber == 0) // because this is not required when we score fluence.
               {

                G4LogicalVolume* small_insert_logic = new G4LogicalVolume(small_insert,Acetal,"small_insert_logic",0,0,0);
                new G4PVPlacement(0,G4ThreeVector(0,0,-1.275*cm),small_insert_logic,"Small_insert",LogicWorld,false,0,fCheckOverlaps);
                
                // The big insert ( needed when scorer 0 and not when shell 0)
                G4Tubs* big_insert = new G4Tubs("big_insert",0.0,gold_rad-0.001,big_insert_len/2,0,360);
                if (fShellnumber != 0) // because we dont use the big insert for the bare detector measurement.
                              {
                               G4LogicalVolume* big_insert_logic = new G4LogicalVolume(big_insert,Acetal,"big_insert_logic",0,0,0);
                               new G4PVPlacement(0,G4ThreeVector(0,0,2.735*cm),big_insert_logic,"Big_insert",LogicWorld,false,0,fCheckOverlaps);
                               }
               }


    //oo00oo............oo00oo................oo00oo............oo00oo.......
    //NNS1
    //NNS1 is a cylinder with the insert volume removed from its core
    G4double Rad1   = 3.6*cm;  //this is the outer radius of the first shell
    G4double len1   = 6.3139*cm; // this is the length of the first shell with the lids(actual value is 6.37 but adjusted according to len2)
    G4double tube_len = 8.01 * cm; // the whole inside the shell 1
    G4Tubs* tube = new G4Tubs ("tube",0.0,gold_rad-0.001,tube_len/2,0,360);
 
  
    G4Tubs* shell1 = new G4Tubs("shell1",0,Rad1,len1/2,0,360);  //length needs to be given as half length. 
    G4VSolid* nns1 = new G4SubtractionSolid("nns1", shell1, tube, 0, G4ThreeVector(0.0, 0.0, 1.46*cm));
    //oo00oo............oo00oo................oo00oo............oo00oo.......
    //NNS2
    G4double rad2   = 3.68*cm;  // this is the inner radius of the 2nd shell
    G4double Rad2   = 4.25*cm;  //this is the outer radius of the 2nd shell
    G4double thick2 = 0.568*cm; // this is the thickness of the 2nd shell and the lid
    G4double len2   = 6.314*cm; // this is the length of the 2nd shell without the lids
 
  
    G4Tubs* shell2 = new G4Tubs("shell2",rad2,Rad2,len2/2,0,360);
    G4Tubs* lid2l  = new G4Tubs("lid2l",0.0*cm,Rad2,thick2/2,0,360);
    G4Tubs* lid2r  = new G4Tubs("lid2r",gold_rad,Rad2,thick2/2,0,360); // a hole is made on the right lid, like in reality
    G4VSolid* lid2 = new G4UnionSolid("lid2", lid2l, lid2r, 0, G4ThreeVector(0.0, 0.0, (len2+thick2)));
    G4VSolid* nns2 = new G4UnionSolid("nns2", shell2, lid2, 0, G4ThreeVector(0.0, 0.0, -(len2+thick2)/2));
    //oo00oo............oo00oo................oo00oo............oo00oo.......
    //NNS3
    G4double rad3   = 4.31*cm;  // this is the inner radius of the 3rd shell
    G4double Rad3   = 4.82*cm;  //this is the outer radius of the 3rd shell
    G4double thick3 = 0.515*cm; // this is the thickness of the 3rd shell and the lid
    G4double len3   = 7.59*cm;  // this is the length of the 3rd shell without the lids
 
    G4Tubs* shell3 = new G4Tubs("shell3",rad3,Rad3,len3/2,0,360);
    G4Tubs* lid3l  = new G4Tubs("lid3l",0.0*cm,Rad3,thick3/2,0,360);
    G4Tubs* lid3r  = new G4Tubs("lid3r",gold_rad,Rad3,thick3/2,0,360); // a hole is made on the right lid, like in reality
    G4VSolid* lid3 = new G4UnionSolid("lid3", lid3l, lid3r, 0, G4ThreeVector(0.0, 0.0, (len3+thick3)));
    G4VSolid* nns3 = new G4UnionSolid("nns3", shell3, lid3, 0, G4ThreeVector(0.0, 0.0, -(len3+thick3)/2));
    //oo00oo............oo00oo................oo00oo............oo00oo.......
    //NNS4
    G4double rad4   = 4.78*cm; // this is the inner radius of the 4 shell
    G4double Rad4   = 5.9*cm;  //this is the outer radius of the 4 shell
    G4double thick4 = 1.12*cm; // this is the thickness of the 4 shell and the lid
    G4double len4   = 8.58*cm; // this is the length of the 4 shell without the lids
 
    G4Tubs* shell4 = new G4Tubs("shell4",rad4,Rad4,len4/2,0,360);
    G4Tubs* lid4l  = new G4Tubs("lid4l",0.0*cm,Rad4,thick4/2,0,360);
    G4Tubs* lid4r  = new G4Tubs("lid4r",gold_rad,Rad4,thick4/2,0,360); // a hole is made on the right lid, like in reality
    G4VSolid* lid4 = new G4UnionSolid("lid4", lid4l, lid4r, 0, G4ThreeVector(0.0, 0.0, (len4+thick4)));
    G4VSolid* nns4 = new G4UnionSolid("nns4", shell4, lid4, 0, G4ThreeVector(0.0, 0.0, -(len4+thick4)/2));
    //oo00oo............oo00oo................oo00oo............oo00oo.......
    //NNS5
    G4double rad5   = 5.89*cm;  // this is the inner radius of the 5 shell
    G4double Rad5   = 7.0*cm;   //this is the outer radius of the 5 shell
    G4double thick5 = 1.11*cm;  // this is the thickness of the 5 shell and the lid
    G4double len5   = 10.69*cm; // this is the length of the 5 shell without the lids
 
    G4Tubs* shell5 = new G4Tubs("shell5",rad5,Rad5,len5/2,0,360);
    G4Tubs* lid5l  = new G4Tubs("lid5l",0.0*cm,Rad5,thick5/2,0,360);
    G4Tubs* lid5r  = new G4Tubs("lid5r",0.0*cm,Rad5,thick5/2,0,360); // lids without holes
    G4VSolid* lid5 = new G4UnionSolid("lid5", lid5l, lid5r, 0, G4ThreeVector(0.0, 0.0, (len5+thick5)));
    G4VSolid* nns5 = new G4UnionSolid("nns5", shell5, lid5, 0, G4ThreeVector(0.0, 0.0, -(len5+thick5)/2));
    //oo00oo............oo00oo................oo00oo............oo00oo.......
    //NNS6
    G4double rad6   = 7.09*cm; // this is the inner radius of the 6 shell
    G4double Rad6   = 9.31*cm; //this is the outer radius of the 6 shell
    G4double thick6 = 2.22*cm; // this is the thickness of the 6 shell and the lid
    G4double len6   = 12.8*cm; // this is the length of the 6 shell without the lids
 
    G4Tubs* shell6 = new G4Tubs("shell6",rad6,Rad6,len6/2,0,360);
    G4Tubs* lid6l  = new G4Tubs("lid6l",0.0*cm,Rad6,thick6/2,0,360);
    G4Tubs* lid6r  = new G4Tubs("lid6r",0.0*cm,Rad6,thick6/2,0,360);
    G4VSolid* lid6 = new G4UnionSolid("lid6", lid6l, lid6r, 0, G4ThreeVector(0.0, 0.0, (len6+thick6)));
    G4VSolid* nns6 = new G4UnionSolid("nns6", shell6, lid6, 0, G4ThreeVector(0.0, 0.0, -(len6+thick6)/2));
    //oo00oo............oo00oo................oo00oo............oo00oo.......
    //NNS7
    G4double rad7   = 9.29*cm;  // this is the inner radius of the 7 shell
    G4double Rad7   = 11.52*cm; //this is the outer radius of the 7 shell
    G4double thick7 = 2.23*cm;  // this is the thickness of the 7 shell and the lid
    G4double len7   = 17.08*cm; // this is the length of the 7 shell without the lids
 
    G4Tubs* shell7 = new G4Tubs("shell7",rad7,Rad7,len7/2,0,360);
    G4Tubs* lid7l  = new G4Tubs("lid7l",0.0*cm,Rad7,thick7/2,0,360);
    G4Tubs* lid7r  = new G4Tubs("lid7r",0.0*cm,Rad7,thick7/2,0,360);
    G4VSolid* lid7 = new G4UnionSolid("lid7", lid7l, lid7r, 0, G4ThreeVector(0.0, 0.0, (len7+thick7)));
    G4VSolid* nns7 = new G4UnionSolid("nns7", shell7, lid7, 0, G4ThreeVector(0.0, 0.0, -(len7+thick7)/2));
    //oo00oo............oo00oo................oo00oo............oo00oo.......
    G4VSolid* nns12      = new G4UnionSolid("nns12", nns1, nns2, 0, G4ThreeVector(0.0, 0.0, 0.0*cm));
    G4VSolid* nns123     = new G4UnionSolid("nns123",nns12, nns3,0,G4ThreeVector(0.0,0.0,0.0*cm));
    G4VSolid* nns1234    = new G4UnionSolid("nns1234",nns123, nns4,0,G4ThreeVector(0.0,0.0,0.0*cm));
    G4VSolid* nns12345   = new G4UnionSolid("nns12345",nns1234, nns5,0,G4ThreeVector(0.0,0.0,0.0*cm));
    G4VSolid* nns123456  = new G4UnionSolid("nns123456",nns12345, nns6,0,G4ThreeVector(0.0,0.0,0.0*cm));
    G4VSolid* nns1234567 = new G4UnionSolid("nns1234567",nns123456, nns7,0,G4ThreeVector(0.0,0.0,0.0*cm));

    
    // The logic volume and physical volume for NNS are set according to the user provided number of shells.
    if (fScorerNumber == 0){
         switch (fShellnumber){
         	  case 0 : {break;}
              case 1 : {G4LogicalVolume* nns_logic= new G4LogicalVolume(nns1,NNS_mat,"nns_logic",0,0,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,0),nns_logic,"nns",LogicWorld,false,0,fCheckOverlaps); break;}
              case 2 : {G4LogicalVolume* nns_logic= new G4LogicalVolume(nns12,NNS_mat,"nns_logic",0,0,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,0),nns_logic,"nns",LogicWorld,false,0,fCheckOverlaps); break;}
              case 3 : {G4LogicalVolume* nns_logic= new G4LogicalVolume(nns123,NNS_mat,"nns_logic",0,0,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,0),nns_logic,"nns",LogicWorld,false,0,fCheckOverlaps); break;}
              case 4 : {G4LogicalVolume* nns_logic= new G4LogicalVolume(nns1234,NNS_mat,"nns_logic",0,0,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,0),nns_logic,"nns",LogicWorld,false,0,fCheckOverlaps); break;}
              case 5 : {G4LogicalVolume* nns_logic= new G4LogicalVolume(nns12345,NNS_mat,"nns_logic",0,0,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,0),nns_logic,"nns",LogicWorld,false,0,fCheckOverlaps); break;}
              case 6 : {G4LogicalVolume* nns_logic= new G4LogicalVolume(nns123456,NNS_mat,"nns_logic",0,0,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,0),nns_logic,"nns",LogicWorld,false,0,fCheckOverlaps); break;}
              case 7 : {G4LogicalVolume* nns_logic= new G4LogicalVolume(nns1234567,NNS_mat,"nns_logic",0,0,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,0),nns_logic,"nns",LogicWorld,false,0,fCheckOverlaps); break;}
              } 
         }
    else {
          foil_mat = vacuum; // change the material of the scoring volume to vaccum to score cell flux.
         }
    
    //==========================================================================
    // 3. Gold volume
    //==========================================================================
    G4Tubs* goldfoil                = new G4Tubs("goldfoil",0.0*cm,gold_rad,gold_thick/2,0,360);
    G4LogicalVolume* LogicGold      = new G4LogicalVolume(goldfoil,foil_mat,"goldfoil",0,0,0);
    new G4PVPlacement(0,G4ThreeVector(0,0,0),LogicGold,"goldfoil",LogicWorld,false,0);
  

        
    G4double flatSurfaceArea   = 2* (pi*gold_rad*gold_rad); 
    G4double curvedSurfaceArea = (2*pi*gold_rad*gold_thick);
    fGoldArea = flatSurfaceArea + curvedSurfaceArea; //Total surface area of the gold foil
    G4double volume = (pi*gold_rad*gold_rad) * gold_thick;
    fGoldMass = goldfoil_density * volume ; //total mass of the foil



    // Setting visual attributes to the Goldfoil.                                       
    // Visualization attributes
    G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(false);
    LogicWorld->SetVisAttributes(visAttributes);
  
    G4VisAttributes* visgold = new G4VisAttributes(G4Colour(255.0,0,0));
    visgold->SetVisibility(true);
    LogicGold->SetVisAttributes(visgold);
    

    // Always return the physical World
    return fWorldVolume ;
   }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *DetectorConstruction::GetWorldVolume() {
   return fWorldVolume ;
}



