///----------APS Analysis 
//----------Author's Name:Jingbo WANG
//----------Copyright:Those valid for ANL
//----------Modified:22/07/2014
////  Run this Code by command :
//		(1) ./lappd inputfile XXX.root parameterfile
//		(2) ./lappd inputfile XXX.root parameterfile UVeventnumber
//		(3) ./lappd inputfile XXX.root parameterfile StartUVeventNumber EndUVeventNumber
//
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <sys/types.h>
#include "math.h"
#include <string.h>
#include <string>
#ifndef __CINT__
#include "TROOT.h"
#include "TFrame.h"
#include "TFile.h" 
#include "TChain.h"
#include <Riostream.h>
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TTree.h"
#include "TF1.h"
#include "TPostScript.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStopwatch.h"
#include "Calibration.h"
#include "TSplineFit.h"
#include "UVevent.h"
#include "TemplateFit.h"
#include "TRandom3.h"
//added*
/*
#include <ctime>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "nnls.h"
#include "TH1D.h"
#include "TFile.h"
#include "TCanvas.h"
//*added
*/
using namespace std;
using std::cout;
using std::endl;
#endif

char*	treename="lappd"; 
int		UsersEntries = 100;						// anslyze a user defined number of UVevents
int		StartEntry = 0;							// if(argc==4) Start analysis from this entry
int		EndEntry = 100;							// if(argc==4) End analysis at this entry

//------------------Parameter file format-------------------------------------------// 
int				InputFileFormat = 1;
int				Nacquisition = 10;						// Number of acquisitions
int				NfastFrame = 1000;				
		// 999 UVevents per acquisition
int				NChannel = 60;								// number of channels

float			WavSamplingRate = 1.e10;				//Sampling rate
float			WavDeltat = 100;
int				WavDimSize = 256;							// 1032 samples per waveform

int				NtrigChannel = 2;
vector<int>		WhichTrig;

int				IfSelectChannel = 0;
int				NChanSelected = 0;
vector<int>		WhichChannel;

int				IfDynamicWindow = 1;
float			FixTrigTwindow_min = 0, FixTrigTwindow_max = 25600;
float			FixMcpTwindow_min = 0, FixMcpTwindow_max = 25600;

int				DoFFT = 0;
float			TrigCutoffFreq = 1.e9;
float			McpCutoffFreq = 5.e8;

float			TrigFraction_CFD = 0.5;
float			McpFraction_CFD = 0.5;
float			TrigDelay_CFD = 800;
float			McpDelay_CFD = 1000;
int				TrigPointsPerSpline = 1;
int				McpPointsPerSpline = 5;

int				DoAmpCut = 1;
float			AmpThreshold = 50;

int				DoTotCut = 1;
float			TrigTotThreshold = 50;
float			McpTotThreshold = 50;
float			TrigMinimumTot = 800;
float			McpMinimumTot = 1500;

int				DoAnalysis = 0;
int				DoCalibration = 0;
int			MakeTemplate=0;
int			DoTempFit;
int                     WhichTempChannel;

int			WhichChi2=0;
float 			Chi2Cut=5.0;

vector<int>		WhichFitChannel;
vector<float>		TFStartScale;
vector<float>		TFStartOffset;

vector<float>		TFRange;
vector<float>		TSQCut;
vector<float>		GRange;

TRandom3* mrand = new TRandom3(12344222);


void SetParameters(char *par);
//Slewing correction
int		ProfileMode = 0;
int		BinSizeMode = 1;
vector<double> CalibY[20];							// Amplitude
vector<double> CalibX[20];							// Time
void SetVarsForAnalysis(UVevent* fevent);
void Analyze(TFile *f);
void Plot(TFile *f);

//TGraph** lhoods;

//===========================================================================================================
//	Start main() function
//-----------------------------------------------------------------------------------------------------------
int main(int argc, char **argv) {
	
  cout<<"-------------------------------------------------------------------"<<endl;
  cout<<"-------------------------------------------------------------------"<<endl;
  
  //------------------Set input and output filenames-------------------------------------------// 
  
  TStopwatch clock;								//runtime clock
  clock.Start();									//clock started	  
  char *FileInput = "test.fff";			//default input file
  char *FileOutput = "otp.root";			//default output file
  char *FileParameters = "par.txt";
  switch(argc){
  case 1: {cout<<"Error: File name required as the 2nd statement"<<endl; break;} 
  case 2: {FileInput = argv[1]; break;}
  case 3: {FileInput = argv[1]; FileOutput = argv[2]; break;} 
  case 4: {FileInput = argv[1]; FileOutput = argv[2]; FileParameters = argv[3]; break;}
  case 5: {FileInput = argv[1]; FileOutput = argv[2]; FileParameters = argv[3]; UsersEntries = atoi(argv[4]); break;}
  case 6: {FileInput = argv[1]; FileOutput = argv[2]; FileParameters = argv[3]; StartEntry = atoi(argv[4]); EndEntry = atoi(argv[5]); break;}
  }
  cout<<"Input file:"<<"\t"<< FileInput << endl;
  cout<< "Output file:"<<"\t" << FileOutput << endl;
  cout<<"Parameter file:"<<"\t"<< FileParameters << endl;
  
  //--------------------------Set parameters----------------------------------------------------//
  SetParameters(FileParameters);
  cout<<"WhichChi2= "<<WhichChi2<<" Chi2Cut= "<<Chi2Cut<<endl;
  vector<int> polarity;
  for(int i=0;i<NChannel;i++) {polarity.push_back(-1);}	//Set all polarity to -1;
  for(int i=0;i<NtrigChannel;i++) {
    int k = WhichTrig.at(i);
    if(k<=NChannel) polarity.at(k) = 1;					//Set trigger polarity to 1;
    else cout<<"Error: Trigger Channel out of range "<<NChannel<<endl;
  } 
  vector<int> selection;
  for(int i=0;i<NChannel;i++) {selection.push_back(0);}	//Set all channel-flag to 0;
  for(int i=0;i<NChanSelected;i++) {
    int k = WhichChannel.at(i);
    if(k<=NChannel) selection.at(k) = 1;					//Set selected channel-flag to 1;
    else cout<<"Error: Selected Channel out of range "<<NChannel<<endl;
  }
  
  
  //lhoods = new TGraph*[100];
  
  //------------------Create a root file--------------------------------------------------------//
  TFile *f = new TFile(FileOutput, "RECREATE");				//open a file
  TTree lappd(treename, treename);				//create a tree
  UVevent *event = new UVevent();				//point to class UVevent
  lappd.Branch("event", &event, 16000, 99);	//Set branches with "split" option
  f->mkdir("wav");
  f->mkdir("pulses");
  f->mkdir("peds");
  
  if(WhichChi2>0){
    f->mkdir("lowchi");							//histograms of waveform with low chi2
    f->mkdir("highchi");							//histograms of waveform with high chi2
  }
  f->mkdir("cfd");						        //constant fraction discrimination
  f->mkdir("raw");
  f->mkdir("cor");
  
  //---------------------Fill a tree------------------------------------------------------------//
  ifstream isin;
  isin.open(FileInput, ios::in);					//open input .txt files
  float WavSample[NChannel][WavDimSize];
  int Nentries = Nacquisition*NfastFrame;			//number of entries
  
  cout<<"Total # of UVevents: "<<Nentries<<endl;
  
  int nww=0; //number of waves written to root file
  int nlc=0; //number of lowchi2 waves to write
  int nhc=0; //number of highchi2 waves to write
  
  TH1D* smoothedtemplate;
  TH1D* thetemplate;
  TemplateFit* mtf = new TemplateFit("fit");
  TemplateFit* mtf2;
  
  if(MakeTemplate==2){
    DoTempFit=1;
    mtf2 = new TemplateFit("fit2");
  }
  
  if(DoTempFit==1){
    TFile* tf = new TFile("template.root");
    thetemplate = (TH1D*) tf->Get("thetemplate");
    
    // Set Fit Range
    mtf->SetTempRange(TFRange.at(0),(TFRange.at(1)));
    mtf->SetFitRange(TFRange.at(0),(TFRange.at(1)));
    mtf->CalcFixedDegFree(thetemplate);
    // currently using a version of the template, smoothed using polynomial interpolation,
    // to provide finer binning than the sampling rate of the scope (or chip)_
    mtf->AddSmoothedTemplate(thetemplate);
    
    // save the smoothed template
    smoothedtemplate = mtf->ReturnGTemplate();
    f->cd();
    smoothedtemplate->Write("smoothedtemplate");
  }
  
  for (int i=0;i<Nentries;i++) {
    if(argc==5) {if(i>UsersEntries) break;}
    if(argc==6) {if(i<StartEntry) continue; if(i>EndEntry) break;}
    clock.Stop();
    if(i%100 == 0) cout<<"processing "<<i<<"th UVevent..."<<"\t"<<"CpuTime = "<<clock.CpuTime()<<" s"<<endl;
    clock.Continue();		
    event->Setevtno(i);						//Set UVevent number
    event->SetWavDimSize(WavDimSize);
    event->SetWavSampleRate(WavSamplingRate);
    event->Initialize();
    
    //------------------------------------read scope data file---------------------------------------------
    if(InputFileFormat == 0) {
      for(int m=0;m<NChannel;m++) {
	for(int n=0;n<WavDimSize;n++) {	
	  isin>>WavSample[m][n];
	  WavSample[m][n] *= polarity.at(m);
	}
      }
    }
    //------------------------------------read psec data file---------------------------------------------
    if(InputFileFormat == 1) {
      int nsa = 0;					//read waveforms
      float meta = 0;
      for(int n=0;n<WavDimSize;n++) {
	isin>>nsa;
	for(int m=0;m<NChannel/2;m++) {
	  isin>>WavSample[m][n];
	  WavSample[m][n] *= -polarity.at(m);
	}
	isin>>meta;
	isin>>nsa;
	for(int m=NChannel-1;m>=NChannel/2;m--) {
	  isin>>WavSample[m][n];
	  WavSample[m][n] *= -polarity.at(m);
	}
	isin>>meta;
      }
    }
    //------------------------------------------------------------------------------------------------------			
    int fnwav = 0;
    for(int m=0;m<NChannel;m++) {				//loop over channels								
      
      int nottrig=-1;
      if(NtrigChannel==0) nottrig=m;
      else if(m!=WhichTrig.at(0)) nottrig=m;
      
      
      if(IfSelectChannel==1) {if(selection.at(m)==0) continue;}
      int WavID = (i+1)*100+m;						//Waveform ID = evtno*100+ChannalNum
      Waveform *waveform = (Waveform*)(event->fwav->ConstructedAt(event->fnwav++));
      waveform->Setevtno(i);
      waveform->Setchno(m);
      waveform->SetSamplingrate(WavSamplingRate);
      waveform->SetDeltat(WavDeltat);
      waveform->SetDimSize(WavDimSize);
      waveform->Initialize(&WavSample[m][0], nottrig);
      waveform->SetAmpThreshold(AmpThreshold);
      if(polarity.at(m)==1) {					//trigger
	waveform->SetMinimumTot(TrigMinimumTot);
	waveform->SetTotThreshold(TrigTotThreshold);
	waveform->SetCutoffFrequency(TrigCutoffFreq);
	waveform->SetPointsPerSpline(TrigPointsPerSpline);
	waveform->SetFraction_CFD(TrigFraction_CFD);
	waveform->SetDelay_CFD(TrigDelay_CFD);
	waveform->SetFitWindow(FixTrigTwindow_min, FixTrigTwindow_max);
      }
      else {									//mcp
	waveform->SetMinimumTot(McpMinimumTot);
	waveform->SetTotThreshold(McpTotThreshold);
	waveform->SetCutoffFrequency(McpCutoffFreq);
	waveform->SetPointsPerSpline(McpPointsPerSpline);
	waveform->SetFraction_CFD(McpFraction_CFD);
	waveform->SetDelay_CFD(McpDelay_CFD);
	waveform->SetFitWindow(FixMcpTwindow_min, FixMcpTwindow_max);
	// Set Gauss Range
	waveform->SetGaussRange(GRange.at(0),GRange.at(1));
	
      }
      if(DoFFT==1) waveform->EnableFFT();
      if(IfDynamicWindow == 1) waveform->EnableDynamicWindow();
      
      
      //	if((waveform->npeaks)==1){ cout<<"analyze chan "<<m<<endl; }
      
      
      if(DoTempFit==1){
	// mtf->SetNoise(waveform->bnoise);
	mtf->SetNoise(3.0);
	
	
	//set starting values for temp fit
	for(int p=0; p<NChannel-NtrigChannel; p++){
	  
	  if(m==WhichFitChannel.at(p)){ 
	    double startoffset = TFStartOffset.at(p)*(mrand->Rndm()-0.5);
	    mtf->SetStartingValues(startoffset,TFStartScale.at(p));
	    //cout<<"Starting Values: "<<m<<" "<<WhichFitChannel.at(p)<<" "<<TFStartOffset.at(p)<<" "<<TFStartScale.at(p)<<endl;
	  }
	}	
	// The analysis
	if(nottrig) {waveform->Analyze(mtf);} 
	else waveform->Analyze(); //analyze waveform
      }
      else{
	waveform->Analyze();					//analyze waveform
      }
      
      
      //	if((waveform->npeaks)==1){ cout<<"done it "<<m<<endl; }
      
      
      // write the first 20 waveforms and cfds, regardless of whether there is a pulse
      if(i<50){
	f->cd("wav");
	waveform->hwav->Write();
	waveform->hwav_raw->Write();
	waveform->xvector->Write();
	waveform->nnlsoutput->Write();
	//waveform->hbg->Write();
	f->cd("cfd");
	//waveform->hcfd->Write();
      }
      
      // if it's a bad baseline (see baseline2 in UVevent), we save the graph in the peds folder
      if(!waveform->goodbl) {
	f->cd("peds");
	waveform->hpedhist->Write();
      }
      
      
      // require one and only one peak
      if((waveform->npeaks)>0){ 
	
	
	double mtt,mts;
	
	if(MakeTemplate==1){	//and waveforms to the template maker
	  if((waveform->gsigma>TSQCut.at(0))&&(waveform->gsigma<TSQCut.at(1))){
	    if(m==WhichTempChannel) { mtf->AddTemplate(waveform->hwav);}
	  }
	}
	
	
	if(MakeTemplate==2){ //fit waveforms with exsting template, line waveforms up, and make new template
	  if((waveform->gsigma<TSQCut.at(0))&&(waveform->gsigma<TSQCut.at(1))){
	    mtt = waveform->ttime;
	    mts = waveform->tscale;
	    TH1D* bfwf = mtf2->ShiftAndScale((waveform->hwav),-mtt,1.0);
	    if(m==WhichTempChannel) { mtf2->AddTemplate(bfwf);}
	    delete bfwf;
	  }
	}
	
	if(nottrig>-1){
	  nww++; 
	  
	  if(nww<100){
	    f->cd("pulses");		    //go to folder "/pulses"
	    waveform->hwav->Write();	    //write histograms for waveforms
	    
	    if(DoTempFit){
	      
	      // if we are doing the template fit, save the template at it's best fit value
	      mtt = waveform->ttime;
	      mts = waveform->tscale;
	      //cout<<"hey "<<i<<" "<<m<<" "<<mtt<<" "<<mts<<endl;
	      
	      //TH1D* bftemp1 = mtf->ShiftAndScale(thetemplate,mtt,mts);
	      TH1D* bftemp1 = mtf->ShiftAndScale(smoothedtemplate,mtt,mts);
	      TString tname;
	      tname+="fit";
	      tname+=i;
	      tname+="_";
	      tname+=m;
	      bftemp1->Write(tname);
	      
	    }
	  }
	  
	  // store the waveforms into different folders, based on the quality of their gaussian/template likelihoods
	  if(WhichChi2>0){
	    
	    float _chi2=0;
	    if(WhichChi2==2) _chi2=(waveform->gchi2);
	    else _chi2=(waveform->tchi2);
	    
	    
	    if( (_chi2<Chi2Cut) && (nlc<100) ){
	      nlc++;
	      f->cd("lowchi");
	      waveform->hwav->Write();
	    }
	    if( (_chi2>Chi2Cut) && (nhc<100) ){
	      nhc++;
	      f->cd("highchi");
	      waveform->hwav->Write();
	    }
	  }
	}
      }
      
    }
    if(DoAnalysis) SetVarsForAnalysis(event);
    
    cout<<"beforefill"<<endl;
    lappd.Fill();								//Fill tree	
    cout<<"afterfill"<<endl;
    event->Clear();	 							//Clear the event
    
  }
  
  f->cd();									//home folder
  if(DoAnalysis) Analyze(f);									
  lappd.Write();									//write tree
  isin.close();
  
  if(MakeTemplate==1){								//save the newly made template
    thetemplate = mtf->ReturnTemplate(true);
    TFile *tf = new TFile("template.root","RECREATE");
    thetemplate->Write("thetemplate");
    tf->Close();
  }
  
  if(MakeTemplate==2){								//save the newly made template
    thetemplate = mtf2->ReturnTemplate(true);
    TFile *tf = new TFile("template.root","RECREATE");
    thetemplate->Write("thetemplate");
    tf->Close();
  }
  
  
  cout<<"-------------------------------------------------------------------"<<endl;
  cout<<"-------------------------------------------------------------------"<<endl;
  
  f->Close();
  //	delete f; f=0;
  return 0;
}
//-----------------------------------------------------------------------------------------------------------
//	End main() function
//===========================================================================================================

void SetParameters(char* par) {
	string name;
	int fileopt, k;
	float kk;
	ifstream parin;
	parin.open(par, ios::in);					//open input .txt files
	parin>>name>>InputFileFormat>>name>>Nacquisition>>name>>NfastFrame>>name>>NChannel;
	parin>>name>>WavSamplingRate>>name>>WavDeltat>>name>>WavDimSize;
	parin>>name>>NtrigChannel>>name;
	for(int i=0;i<NtrigChannel;i++) {
		parin>>k;
		WhichTrig.push_back(k);
	}
	parin>>name;
	parin>>IfSelectChannel>>name>>NChanSelected>>name;
	for(int i=0;i<NChanSelected;i++) {
		parin>>k;
		WhichChannel.push_back(k);
	}
	parin>>name>>IfDynamicWindow>>name>>FixTrigTwindow_min>>FixTrigTwindow_max>>name>>FixMcpTwindow_min>>FixMcpTwindow_max;
	parin>>name>>DoFFT>>name>>TrigCutoffFreq>>name>>McpCutoffFreq;
	parin>>name>>TrigFraction_CFD>>name>>McpFraction_CFD>>name>>TrigDelay_CFD>>name>>McpDelay_CFD>>name>>TrigPointsPerSpline>>name>>McpPointsPerSpline;
	parin>>name>>DoAmpCut>>name>>AmpThreshold;
	parin>>name>>DoTotCut>>name>>TrigTotThreshold>>name>>McpTotThreshold>>name>>TrigMinimumTot>>name>>McpMinimumTot;
	parin>>name>>DoAnalysis>>name>>MakeTemplate>>name>>DoTempFit>>name>>WhichTempChannel;

	cout<<"ncs "<<NChanSelected<<" "<<NtrigChannel<<endl;

	cout<<"Do T Fit "<<DoTempFit<<" MakeTemp: "<<MakeTemplate<<" "<<McpMinimumTot<<" DoAnalysis "<<DoAnalysis<<endl;



	parin>>name;
	for(int i=0;i<(NChanSelected-NtrigChannel);i++){
		parin>>k;
		WhichFitChannel.push_back(k);
		cout<<"Rrr "<<k<<endl;
	}
	parin>>name;
	for(int i=0;i<(NChanSelected-NtrigChannel);i++){
		parin>>kk;
		TFStartOffset.push_back(kk);

	}
	parin>>name;
	for(int i=0;i<(NChanSelected-NtrigChannel);i++){
		parin>>kk;
		TFStartScale.push_back(kk);
	}

	parin>>name;
	for(int i=0;i<2;i++){
		parin>>kk;
		TFRange.push_back(kk);
	}
	parin>>name;
	for(int i=0;i<2;i++){
		parin>>kk;
		TSQCut.push_back(kk);
	}
	parin>>name;
	for(int i=0;i<2;i++){
		parin>>kk;
		GRange.push_back(kk);
	}

	parin>>name>>WhichChi2;
	parin>>name>>Chi2Cut;

}

//----------------------------------------------------------------------------------------------
//-------------------This part has to be modified by the user----------------------------------
//----------------------------------------------------------------------------------------------
void SetVarsForAnalysis(UVevent* fevent) {
	//--------- Calculate variables of interest ------
	float t[4], q[4];
	t[0] = ((Waveform*)(fevent->fwav->ConstructedAt(0)))->time;
	t[1] = ((Waveform*)(fevent->fwav->ConstructedAt(1)))->time;
}

void Analyze(TFile *f) {
	cout<<"-------------------------Come to Analysis------------------------------------------"<<endl;
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void Plot(TFile *f) {
	gStyle->SetOptFit(111);
	gStyle->SetPalette(1);
	
}
