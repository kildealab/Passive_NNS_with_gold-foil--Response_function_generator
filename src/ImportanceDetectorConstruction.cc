
//

#include "globals.hh"
#include <sstream>
#include <cmath>
#include "G4UIcommand.hh"

#include "ImportanceDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
//Visualization
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

// For Primitive Scorers
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"


#include "G4VPrimitiveScorer.hh"
#include "nCaptureScorer.hh"
#include "nCellFluxScorer.hh"
#include "nFluenceScorer.hh"


// for importance biasing
#include "G4IStore.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int Num = 0; //Global initialization of the Num of cells

ImportanceDetectorConstruction::ImportanceDetectorConstruction(G4String worldName) 
    :G4VUserParallelWorld(worldName),
     fLogicalVolumeVector(),
     fTubVector(),
     fShellnumber(0),
     fScorerNumber(0),
     fNum()
     {  } //  Construct(); 
  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ImportanceDetectorConstruction::~ImportanceDetectorConstruction()
    {
     fLogicalVolumeVector.clear();
    }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ImportanceDetectorConstruction::Construct()
   {  
    G4Material* dummyMat  = 0; //initialize dummy material, because the material is obtained from the mass world
    auto scoring_mat = dummyMat;
    auto shell_mat = dummyMat;

    if(fScorerNumber == 1) //switch the material of the scoring region and the shells to vacuum for fluence calculation
      {auto vacuum   = G4Material::GetMaterial("G4_Galactic");
       scoring_mat = vacuum;
       shell_mat = vacuum;}

    
    
    //GetWorld methods create a clone of the mass world to the parallel world (!)
    // via the transportation manager
    fGhostWorld = GetWorld();

    G4LogicalVolume* worldLogical = fGhostWorld->GetLogicalVolume();
    fLogicalVolumeVector.push_back(worldLogical);
    fPVolumeStore.AddPVolume(G4GeometryCell(*fGhostWorld, 0)); //adds the world volume to the volume store

    G4double Moderator_real [] = {0.0,3.33,3.898,4.413,5.533,6.643,8.863,11.093}; // these are the radius/halflength of each moderator shells. Maximum dimension from the centre.
    G4double RealSize = Moderator_real[fShellnumber]; //this gets the size of the moderator configuration in the simulation.


    G4double thickness = 1.0*cm;//Thickness of one importance region. This cant be less than the radius of the foil(0.8 cm)
    Num = ceil(RealSize*cm/thickness); // Num is the no of devisions or no of impotance regions. 
    Num = Num+1; //Now, Num also includes the number of divisions + gold foil volume.
    
    G4String name = "none";
    G4double width =0;

    //Importance geometry is constructed with the for loop.
    //Importance regions are concentric cylinders with uniform thickness.
    for (G4int idx =1; idx<Num+1 ; idx++)
	    {
	     if(idx==1)
		    {//if fShellNumber is zero, then construct only the gold foil 
			 name = GetCellName(idx);
			 G4Tubs* foil = new G4Tubs("foil",0.0*cm,0.8*cm,0.005*cm,0,360); //get the dimensions from detector construction
			 fTubVector.push_back(foil);
			 
             G4LogicalVolume* LogicGold      = new G4LogicalVolume(foil,scoring_mat,"foil",0,0,0);
			 fLogicalVolumeVector.push_back(LogicGold);
			 
             G4VPhysicalVolume* Pfoil = new G4PVPlacement(0,G4ThreeVector(0,0,0),LogicGold,name,worldLogical,false,idx);
			 fPVolumeStore.AddPVolume(G4GeometryCell(*Pfoil, idx));
	    	}
	     else
           { 
            width = thickness * (idx-1); //its the width from center to the boundary for each regions. 
			G4Tubs* cylinder = new G4Tubs("cylinder",0,width,width,0,360); // this would be a filled cylinder
			G4VSolid* SolidCell= new G4SubtractionSolid("cell",cylinder,fTubVector[idx-2],0,G4ThreeVector(0,0,0)); //solidcell = cylinder - previous cylinder
			fTubVector.push_back(cylinder);//we pass the cylinder to the vector, so that the subtraction vector will remove the entire region inside the new cylinder.

			name = GetCellName(idx);
			G4String end_term = G4UIcommand::ConvertToString(idx); //convert the number to a string to give unique name for the logical volume
			G4LogicalVolume *LogicCell = new G4LogicalVolume(SolidCell,shell_mat,"cell_logic"+end_term,0,0,0);
			fLogicalVolumeVector.push_back(LogicCell);

			G4VPhysicalVolume* PCell = new G4PVPlacement(0,G4ThreeVector(0,0,0),LogicCell,name,worldLogical,false,idx);
    		G4GeometryCell cell(*PCell, idx);
    		fPVolumeStore.AddPVolume(cell);
    	   }
	   }


    //Set the visual attributes to see the mass world and the parallel world together.
    G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(false);
    worldLogical->SetVisAttributes(visAttributes);
 

   }



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String ImportanceDetectorConstruction::GetCellName(G4int i)
   {
    std::ostringstream os;
    os << "cell_";
    if (i<10) 
        {
         os << "0";
        }
    os << i;
    G4String name = os.str();
    return name;
   }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4GeometryCell ImportanceDetectorConstruction::GetGeometryCell(G4int i)
   {
    G4String name(GetCellName(i));
    const G4VPhysicalVolume *p=0;
    p = fPVolumeStore.GetPVolume(name);
    if (p) 
       {
        return G4GeometryCell(*p,0);
       }
    else 
       {
        G4cout << "B02ImportanceDetectorConstruction::GetGeometryCell: " << G4endl
               << " couldn't get G4GeometryCell" << G4endl;
        return G4GeometryCell(*fGhostWorld,-2);
       }
   }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume &ImportanceDetectorConstruction::GetWorldVolumeAddress() const
   {
    return *fGhostWorld;
   }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *ImportanceDetectorConstruction::GetWorldVolume() 
   {
    return fGhostWorld; 
   }


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ImportanceDetectorConstruction::ConstructSD()
   { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    // Sensitive Detector Name
    G4String concreteSDname = "mfDetector";

    //------------------------
    // MultiFunctionalDetector
    //------------------------
    //
    // Define MultiFunctionalDetector with name.
    G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(concreteSDname);
    SDman->AddNewDetector( MFDet ); // Register SD to SDManager


    SetSensitiveDetector(fLogicalVolumeVector[1]->GetName(), MFDet);//sets the second logical volume in the vector (foil) as sensitive. fLogicalVolumeVector[0] is the world.
    G4cout<<G4endl<<"The sensitive detector is \""<<fLogicalVolumeVector[1]->GetName()<<"\""<<G4endl<<G4endl;

    if(fScorerNumber == 0)
      {
       G4VPrimitiveScorer*   ncapturescore = new nCaptureScorer("nCapture");  
       MFDet->RegisterPrimitive(ncapturescore);
      }

    if(fScorerNumber == 1)
      {
       nCellFluxScorer* CellFlux = new nCellFluxScorer("CellFlux"); 
       MFDet->RegisterPrimitive(CellFlux);
      }

    if(fScorerNumber == 2)
      {
       G4VPrimitiveScorer*   nfluencescore = new nFluenceScorer("nFluence");  
       MFDet->RegisterPrimitive(nfluencescore);
      }
   }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



/*****************************************************************************
The importance of each region must be assigned in the following function. 
The World volume should have an importance of 1 and the importance should
increse as we go to the centre of the detector. The region inside which the gold
foil is situated gets an importance of the preceding region and the outermost 
region before the world should have an importance of 1.
******************************************************************************/   
G4VIStore* ImportanceDetectorConstruction::CreateImportanceStore()
   {
    // creating and filling the importance store
  
    G4IStore *istore = G4IStore::GetInstance(GetName());

    G4double imp =0.0;

    // set importance values for each cell in the volume store.
    G4int cell(1);
    for (cell=1; cell<Num+1; cell++) 
        {
         if (cell==1)
            { //Storing importance for the foil with the same importance as the first cell (inner most cell)
             G4GeometryCell gCell = GetGeometryCell(cell);
             G4cout << " adding cell: " << cell 
                    << " replica: " << gCell.GetReplicaNumber() 
                    << " name: Gold foil" << G4endl;
        
             if (Num==1){imp = std::pow(2.0,Num-1);} //this is the case when its bare gold
             else{imp = std::pow(2.0,Num-2);} // this is when we have atleast one moderator shell.

             G4cout << "Going to assign importance: " << imp << ", to volume: Gold foil"  << G4endl;
    
             istore->AddImportanceGeometryCell(imp, gCell.GetPhysicalVolume(), cell); //assign importance
            }
         else
            {
             G4GeometryCell gCell = GetGeometryCell(cell);
             G4cout << " adding cell: " << cell 
                    << " replica: " << gCell.GetReplicaNumber() 
                    << " name: " << gCell.GetPhysicalVolume().GetName() << G4endl;
             imp = std::pow(2.0,Num-cell);

             G4cout << "Going to assign importance: " << imp << ", to volume: " 
                    << gCell.GetPhysicalVolume().GetName() << G4endl;
    
             istore->AddImportanceGeometryCell(imp, gCell.GetPhysicalVolume(), cell);
            }
        }

    // Storing the same importance for the world volume as of the last shell(outer most cell)
    G4GeometryCell gWorldVolumeCell(GetWorldVolumeAddress(), 0);
    istore->AddImportanceGeometryCell(1, gWorldVolumeCell);

    return istore;

   }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


