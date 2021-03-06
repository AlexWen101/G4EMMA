//
// **************************************************************
// *                                                            *
// *                                                            *
// * This program was designed as a GEANT4 front end for the    *
// * BuildGeometry program designed by Derek Howell for use     *
// * in the simulation of EMMA at TRIUMF.                       *
// *                                                            *
// *                                                            *
// * Instructions for use:                                      *
// *    Output files from BuildGeometry including               *
// *                     SpectrometerConstruction.cc / .hh      *
// *                     DriftChamberHit.cc / .hh               *
// *    Should be copied to the appropriate folder              *
// *    And a single instance of the SpectrometerConstruction   *
// *    class is created in the DetectorConstruction file.      *
// *                                                            *
// *                                                            *
// **************************************************************
//
// *************************************************************************************************
//
//
//   Version:		1.7
//   Date:			July 2014
//   Modified by:	Naomi Galinski
//   Original Author:		Derek Howell
//
// Version1.6:
// Now uses ROOT instead of AIDA.
// Input file format changed.
//
// Verstion1.7:
// Changes made to SpectrometerConstruction.cc.:
//   - less input parameters required to make new SpectrometerConstruction object
//   - slits.dat input file format changed.
//   - horizontal slits are used and added right and left slits
//   - got rid of vertical slits
//   - changes made to pipe lengths
//   - Bruker's apertures are used for the quads and EDs and MD have rectangular aperture slits
//   - BGFieldn.cc object definition has changed. More input parameters are required.
//   - uses EMFieldDebugger.cc to debug calculated E and B fields
//   - added lengths and distances used in EMFieldDebugger.cc
// BGField classes:
//   - positions and lengths of fields are now added as input parameters when defining a field
//   - effective field length (EFL) changed so that the output is the correct EFL
//   - changed field strengths of mass=100 AMU q=20 E=180 MeV to new specifications
// Created EMFieldDebugger.cc
//   - prints out elec. and mag. fields in each element
// EMMASteppingAction.cc:
//   - added a debugging section to print out the x and y exit locations along the optical axis
//     at the end of the effective fields
//   - changed the names of volumes to account for the change in names in SpectrometerConstruction
//     and the new volumes
// Debugged EMMADetectorConstruction.cc
//   - the program crashed when the degraders were created with 0 width. Problem solved by creating
//     the degraders inside the 'if' statement, such that the shape is not created if the degraders
//     are specified at 'OUT' in the input file
//   - since the dimensions in the SpectrometerConstruction.cc changed I don't know if the MWPC will
//     work
//   - got rid of unused length and distance parameters
//   - changed input format of targetDegraders.dat
//   - target degrader1, degrader2 and focal plane positions are now fixed
// *************************************************************************************************



#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "EMMADetectorConstruction.hh"
#include "EMMAPhysicsList.hh"
#include "EMMAPrimaryGeneratorAction.hh"

#include "EMMAEventAction.hh"

#include "EMMASteppingAction.hh"
#include "EMMASteppingVerbose.hh"

#include "TrackingAction.hh"
#include "StackingAction.hh"

#include "G4Event.hh"

#include <string> //words and sentences
#include <fstream> //Stream class to both read and write from/to files
#include <sstream>
using namespace std;



#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif



// global variable
G4String MotherDir;
G4String UserDir;
G4int NOHslits1=0;
G4int NOHslits2=0;
G4int NOHslits3=0;
G4int NOHslits4=0;




void ReadUserInput_Beam( G4String &s1, G4String &s2, G4String &s3, G4String &s4, G4String &s5, G4String &s6, G4String &s7, G4String &s8, G4String &s9, G4String &s10);
void ReadUserInput_Reaction( G4String &s1, G4String &s2, G4String &s3, G4String &s4, G4String &s5, G4String &s6,
			     G4String &s7, G4String &s8, G4String &s9, G4String &s10, G4String &s11, G4String &s12,
			     G4double &s13 );
void ReadUserInput_CentralTrajectory( G4String &s1, G4String &s2, G4String &s3, G4String &s4 );


int main(int argc,char** argv)
{

  // input arguments
  G4String vis = "visOFF";
  MotherDir = ".";
  UserDir = MotherDir + "/UserDir/";
  if (argc > 1) vis = argv[1];
  if (argc > 2) MotherDir = argv[2];
  if (argc > 3) UserDir = argv[3];
  G4cout << "Visualisation: " << vis << G4endl;
  G4cout << "Main directory: " << MotherDir << G4endl;
  G4cout << "User directory: " << UserDir << G4endl;


  // User Verbose output class
  //
  G4VSteppingVerbose* verbosity = new EMMASteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);

  // RunManager construction
  G4RunManager* runManager = new G4RunManager;

#ifdef G4VIS_USE
  // Visualization manager construction
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // mandatory user initialization classes
  runManager->SetUserInitialization(new EMMADetectorConstruction); //declare detector parts
  runManager->SetUserInitialization(new EMMAPhysicsList); //declare particles and physics processes

  G4UserSteppingAction* stepping_action = new EMMASteppingAction;
  runManager->SetUserAction(stepping_action);
  TrackingAction* trkAct = new TrackingAction();
  runManager->SetUserAction(trkAct);
  runManager->SetUserAction(new StackingAction);

  // initialize Geant4 kernel
  runManager->Initialize();

  // mandatory user action class
  EMMAPrimaryGeneratorAction* priGenAct = new EMMAPrimaryGeneratorAction();
  runManager->SetUserAction(priGenAct);

  // optional user action classes
  runManager->SetUserAction(new EMMAEventAction);

  // start interactive session
#ifdef G4VIS_USE
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if (vis=="visON") UImanager->ApplyCommand("/control/execute visEMMA.mac");
  //      UImanager->ApplyCommand("/control/execute BeamSetup.mac");


  G4String s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12;
  G4double d13;
  G4String command;
	G4double Theta, Phi;


  //-----------------------------------------
  //            Specify beam
  //-----------------------------------------
  ReadUserInput_Beam(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);
  command = "/mydet/nEvents "; command.append(s1); UImanager->ApplyCommand(command);
  command = "/mydet/beamZ "; command.append(s2); UImanager->ApplyCommand(command);
  command = "/mydet/beamA "; command.append(s3); UImanager->ApplyCommand(command);
  command = "/mydet/beamCharge "; command.append(s4); UImanager->ApplyCommand(command);
  command = "/mydet/energy "; command.append(s5); UImanager->ApplyCommand(command);
  command = "/mydet/sigmaEnergy "; command.append(s6); UImanager->ApplyCommand(command);
  command = "/mydet/beamSpotDiameter "; command.append(s7); UImanager->ApplyCommand(command);
  command = "/mydet/transEmittance "; command.append(s8); UImanager->ApplyCommand(command);
	command = "/mydet/energyData "; command.append(s9); UImanager->ApplyCommand(command);
	command = "/mydet/angularData "; command.append(s10); UImanager->ApplyCommand(command);
  //-----------------------------------------
	// Passing some commands for GeneralParticleSource when beam is simulated
	// the values obtained are based entirely on the read values that are in PrimaryGeneratorAction.cc

	/* This is all stuff useful if you GeneralParticleSource instead of GeneralParticleGun to generate your particles.
	command = "/gps/ang/type iso"; UImanager->ApplyCommand(command);  // it is not iso, change this.
	//command = "/gps/ang/maxtheta "; command.append("3.1564066 rad"); UImanager->ApplyCommand(command);
	command = "/gps/ang/maxtheta "; command.append("3.1564066 rad"); UImanager->ApplyCommand(command);
	command = "/gps/ang/mintheta "; command.append("3.1265934 rad"); UImanager->ApplyCommand(command);
	//command = "/gps/ang/maxphi "; command.append("6.18 rad"); UImanager->ApplyCommand(command);
	command = "/gps/ang/maxphi "; command.append("6.18 rad"); UImanager->ApplyCommand(command);
	command = "/gps/ang/minphi 0. rad"; UImanager->ApplyCommand(command);


	command = "/gps/ene/type Arb"; UImanager->ApplyCommand(command);
	command = "/gps/hist/file UserDir/UserInput/energySpectrum.dat"; UImanager->ApplyCommand(command);
	command = "/gps/hist/inter Lin"; UImanager->ApplyCommand(command);
	*/
	//command = "/gps/position 0. 0. -0.000501 mm"; UImanager->ApplyCommand(command);


  //-----------------------------------------
  //  Nuclear-reaction process: (1+2->3+4)
  //-----------------------------------------
  ReadUserInput_Reaction(s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,d13);
  command = "/twoBodyReaction/Z1 "; command.append(s1); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/A1 "; command.append(s2); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/Z2 "; command.append(s3); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/A2 "; command.append(s4); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/Z3 "; command.append(s5); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/A3 "; command.append(s6); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/Z4 "; command.append(s7); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/A4 "; command.append(s8); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/qmin "; command.append(s9); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/qmax "; command.append(s10); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/Charge3 "; command.append(s11); UImanager->ApplyCommand(command);
  command = "/twoBodyReaction/ExcitationEnergy3 "; command.append(s12); UImanager->ApplyCommand(command);
  //G4double crossSection1234 = d13;
  G4int simtype=0;
  if (s3=="0" && s4=="0") simtype=0; // reaction not specified; simulate beam
  else simtype=1; // reaction specified; simulate reaction
  //-----------------------------------------


  //-----------------------------------------
  //      Specify central trajectory
  //-----------------------------------------
  ReadUserInput_CentralTrajectory(s1,s2,s3,s4);
  command = "/mydet/centralZ "; command.append(s1); UImanager->ApplyCommand(command);
  command = "/mydet/centralA "; command.append(s2); UImanager->ApplyCommand(command);
  command = "/mydet/centralQ "; command.append(s3); UImanager->ApplyCommand(command);
  command = "/mydet/centralE "; command.append(s4); UImanager->ApplyCommand(command);
  UImanager->ApplyCommand("/mydet/updategeo"); // necessary for changes to take effect
  //-----------------------------------------


  // Amount of output info
  //-----------------------------------------
  UImanager->ApplyCommand("/run/verbose      0");
  UImanager->ApplyCommand("/event/verbose    0");
  UImanager->ApplyCommand("/tracking/verbose 0");
  //-----------------------------------------


//-----------------------------------------------------------------------------------------//
  //Following section is commented out so that the simulation doesn't run automatically.
  //To simulate beam run using command: /mydet/doBeam
  //To simulate reaction run using commands: /mydet/doPrepare then /mydet/doBeam
  //Type 'exit' to exit simulation.

  // Run simulation
  //-----------------------------------------
  //if (simtype==0) UImanager->ApplyCommand("/mydet/doBeam"); //simulate beam without reaction
  //else if (simtype==1) {
  //  UImanager->ApplyCommand("/mydet/doPrepare"); //simulate reaction depth of beam
  //  UImanager->ApplyCommand("/mydet/doReaction"); //simulate recoils from reaction depth
  //  if (crossSection1234>0.) UImanager->ApplyCommand("/mydet/doBeam");
  //}
  // Run simulation for both beam particles and recoils
  //UImanager->ApplyCommand("/mydet/doBeam"); //simulate beam without reaction
  //UImanager->ApplyCommand("/mydet/doPrepare"); //simulate reaction depth of beam
  //UImanager->ApplyCommand("/mydet/doReaction"); //simulate recoils from reaction depth
  //-----------------------------------------
//-----------------------------------------------------------------------------------------//


  // Print hit info
  //-----------------------------------------
  G4cout << G4endl;
  G4cout << "Number of hits:" << G4endl;
  G4cout << "Slits 1: " << NOHslits1 << G4endl;
  G4cout << "Slits 2: " << NOHslits2 << G4endl;
  G4cout << "Slits 3: " << NOHslits3 << G4endl;
  G4cout << "Slits 4: " << NOHslits4 << G4endl;
  G4cout << G4endl;
  // print same info to file
  std::ofstream outfile;
  G4String fname = UserDir;
  fname.append("/Results/diagnostics.dat");
  outfile.open(fname);
  outfile << "Number of hits:" << G4endl;
  outfile << "Slits 1: " << NOHslits1 << G4endl;
  outfile << "Slits 2: " << NOHslits2 << G4endl;
  outfile << "Slits 3: " << NOHslits3 << G4endl;
  outfile << "Slits 4: " << NOHslits4 << G4endl;
  outfile.close();
  //-----------------------------------------


#endif
#ifdef G4UI_USE
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  //UImanager->ApplyCommand("/control/execute visEMMA.mac");
  //UImanager->ApplyCommand("/control/execute macros/BeamSetup.mac");
  ui->SessionStart();
  delete ui;
#endif


#ifdef G4VIS_USE
  delete visManager;
#endif

  delete runManager;
  delete verbosity;

  return 0;
}





void ReadUserInput_Beam( G4String &s1, G4String &s2, G4String &s3, G4String &s4, G4String &s5, G4String &s6, G4String &s7, G4String &s8, G4String &s9, G4String &s10)
{
  s1=""; s2=""; s3=""; s4=""; s5=""; s6=""; s7=""; s8=""; s9=""; s10="";
  G4String text, line;
  ifstream inputfil;
  G4String filename = UserDir + "/UserInput/beam.dat";
  inputfil.open ( filename, ios::in );
  if ( inputfil.is_open() ) {
    int n=0;
    while ( inputfil.good() ) {
      inputfil >> text;
      if (text=="#") { // skip comments
	getline (inputfil,line);
      }
      else {
	n = n+1;
	if (n==1) s1 = text; // # of events
	if (n==2) s2 = text; // Z
	if (n==3) s3 = text; // A
	if (n==4) s4 = text; // charge-state
	if (n==5) s5 = text; // energy
	if (n==6) s6 = text; // resolution
	if (n==7) s7 = text; // diameter
	if (n==8) s8 = text; // normalized transverse geometric emittance
	if (n==9) s9 = text; // source of energy data
	if (n==10) s10=text; // type of angular distribution (uniform or not)
      }
    }
    inputfil.close();
  }
  else G4cout << "Unable to open " << filename << G4endl;
  // get units right:
  s5.append(" MeV");
  s7.append(" mm");
  s8.append(" mm");
}


void ReadUserInput_Reaction( G4String &s1, G4String &s2, G4String &s3, G4String &s4, G4String &s5, G4String &s6,
			     G4String &s7, G4String &s8, G4String &s9, G4String &s10, G4String &s11, G4String &s12,
			     G4double &d13 )
{
  s1=""; s2=""; s3=""; s4=""; s5=""; s6=""; s7=""; s8=""; s9=""; s10=""; s11=""; s12=""; d13=0;
  G4String text, line;
  G4double val;
  ifstream inputfil;
  G4String filename = UserDir + "/UserInput/reaction.dat";
  inputfil.open ( filename, ios::in );
  if ( inputfil.is_open() ) {
    int n=0;
    while ( inputfil.good() ) {
      inputfil >> text;
      if (text=="#") { // skip comments
	getline (inputfil,line);
      }
      else {
	n = n+1;
	if (n==1) s1 = text; // Z1
	if (n==2) s2 = text; // A1
	if (n==3) s3 = text; // Z2
	if (n==4) s4 = text; // A2
	if (n==5) s5 = text; // Z3
	if (n==6) s6 = text; // A3
	if (n==7) s7 = text; // Z4
	if (n==8) s8 = text; // A4
	if (n==9) s9 = text; // theta cm min
	if (n==10) s10 = text; // theta cm max
	if (n==11) s11 = text; // charge-state of fragment 3
	if (n==12) s12 = text; // excitation energy of fragment 3
	val = atof(text.c_str());
	if (n==13) d13 = val; // cross section (mb/sr)
      }
    }
    inputfil.close();
  }
  else G4cout << "Unable to open " << filename << G4endl;
  // get units right:
  s9.append(" deg");
  s10.append(" deg");
  s12.append(" MeV");
}


void ReadUserInput_CentralTrajectory( G4String &s1, G4String &s2, G4String &s3, G4String &s4 )
{
  s1="";  s2="";  s3="";  s4="";
  G4String text, line;
  ifstream inputfil;
  G4String filename = UserDir + "/UserInput/centralTrajectory.dat";
  inputfil.open ( filename, ios::in );
  if ( inputfil.is_open() ) {
    int n=0;
    while ( inputfil.good() ) {
      inputfil >> text;
      if (text=="#") { // skip comments
	getline (inputfil,line);
      }
      else {
	n = n+1;
	if (n==1) s1 = text; // Z
	if (n==2) s2 = text; // A
	if (n==3) s3 = text; // charge-state
	if (n==4) s4 = text; // energy
      }
    }
    inputfil.close();
  }
  else G4cout << "Unable to open " << filename << G4endl;
  // get units right:
  s4.append(" MeV");
}
