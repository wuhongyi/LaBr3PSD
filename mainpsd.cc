// mainpsd.cc --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 五 7月  7 17:04:29 2017 (+0800)
// Last-Updated: 五 7月  7 19:18:24 2017 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 2
// URL: http://wuhongyi.cn 

#include "wuReadData.hh"
#include "offline.hh"
#include "pkuFFTW.hh"

#include <iostream>
#include <climits>

#include "RVersion.h"//版本判断
#include "TApplication.h"
#include "TArrow.h"
#include "TAxis.h"
#include "TBenchmark.h"
#include "TBranch.h"
#include "TBrowser.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TColor.h"
#include "TCutG.h"
#include "TDatime.h"
#include "TError.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFormula.h"
#include "TGaxis.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TLine.h"
#include "TList.h"
#include "TLorentzVector.h"
#include "TMarker.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TMatrixDEigen.h"
#include "TMultiGraph.h"
#include "TNtuple.h"
#include "TObject.h"
#include "TPad.h"
#include "TPaveLabel.h"
#include "TPaveStats.h"
#include "TPaveText.h"
#include "TRandom.h"
#include "TRandom1.h"
#include "TRandom2.h"
#include "TRandom3.h"
#include "TRint.h"
#include "TROOT.h"
#include "TSlider.h"
#include "TSpectrum.h"
#include "TSpectrum2.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TTimer.h"
#include "TTimeStamp.h"
#include "TTree.h"
#include "TVector3.h"
#include "TVectorD.h"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char *argv[])
{

  // Create an interactive ROOT application
  TRint *theApp = new TRint("Rint", &argc, argv);

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
  //初始参数
  TString TreeName="tree";//这里为要处理的文件中 tree name
  // create first the chain with all the files
  TChain *fChain=new TChain(TreeName);
  // std::cout << "Creating the chain" << std::endl;
  fChain->SetCacheSize(20*1024*1024);
  TString dir = gSystem->DirName(__FILE__);//获取当前文件main.cc所在路径 
  dir.ReplaceAll("/./","/");
  // std::cout<<dir<<std::endl;
  //=======================================================
  //以下两个选一个： 手动填写root文件所在路径或者直接使用当前文件所在路径
  // gSystem->Setenv("Dir","/home/wuhongyi");//手动填写路径
  // gSystem->Setenv("Dir",dir);//当前文件路径
  int AddFileNumber = wuReadData::ReadValue<int>("AddFileNumber","ReadData.txt");
  // std::cout<<"AddFileNumber: "<<AddFileNumber<<std::endl;
  //=======================================================
  std::string InputFile;
  for (int i = 0; i < AddFileNumber; ++i)
    {
      char InputFileName[1024];
      sprintf(InputFileName,"InputFileName_%d",i);
      InputFile = wuReadData::ReadValue<std::string>(InputFileName,"ReadData.txt");
      fChain->Add(InputFile.c_str());
    }
  // fChain->Print();

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
  Short_t         ch;
  UShort_t        size;
  UInt_t          timestamp;
  UShort_t        dt[65535];   //[size]
  UShort_t        data[65535];   //[size]

  // Int_t           ch;
  // Int_t           size;
  // UInt_t          timestamp;
  // Int_t           dt[65535];   //[size]
  // Int_t           data[65535];   //[size]
  
  TBranch        *b_ch;   //!
  TBranch        *b_size;   //!
  TBranch        *b_timestamp;   //!
  TBranch        *b_dt;   //!
  TBranch        *b_data;   //!
   
  fChain->SetBranchAddress("ch", &ch, &b_ch);
  fChain->SetBranchAddress("size", &size, &b_size);
  fChain->SetBranchAddress("timestamp", &timestamp, &b_timestamp);
  fChain->SetBranchAddress("dt", dt, &b_dt);
  fChain->SetBranchAddress("data", data, &b_data);
   
  std::cout <<std::endl<< "=== Running Hongyi Wu Analysis ===" << std::endl;

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  gBenchmark->Start("tree");//计时开始
  // std::string OutputFileName = wuReadData::ReadValue<string>("OutputFileName","ReadData.txt");
  Long64_t TotalEntry = fChain->GetEntries();//拿到TChain中总entry行数
  std::cout<<"TotalEntry:  "<<TotalEntry<<std::endl;
  
  int SelectChannel = wuReadData::ReadValue<int>("SelectChannel","ReadData.txt");
  int VotoChannel = wuReadData::ReadValue<int>("VotoChannel","ReadData.txt");
  int VotoTime = wuReadData::ReadValue<int>("VotoTime","ReadData.txt");
  offline *off = new offline();

  std::string OutputFileName = wuReadData::ReadValue<std::string>("OutputFileName","ReadData.txt");
  
  bool PulsePolarity;
  if(wuReadData::ReadValue<int>("PulsePolarity","ReadData.txt") == 1)
    PulsePolarity = true;
  else
    PulsePolarity = false;
  int ADCMSPS = wuReadData::ReadValue<int>("ADCMSPS","ReadData.txt");

  int CalculateVertexPoint = wuReadData::ReadValue<int>("CalculateVertexPoint","ReadData.txt");
  int CalculateBaselinePoint = wuReadData::ReadValue<int>("CalculateBaselinePoint","ReadData.txt");
  int CalculateRiseTimeType = wuReadData::ReadValue<int>("CalculateRiseTimeType","ReadData.txt");
  double FastFilterFL = wuReadData::ReadValue<double>("FastFilterFL","ReadData.txt");
  double FastFilterFG = wuReadData::ReadValue<double>("FastFilterFG","ReadData.txt");
  int FastFilterThre = wuReadData::ReadValue<int>("FastFilterThre","ReadData.txt");
  double SlowFilterSL = wuReadData::ReadValue<double>("SlowFilterSL","ReadData.txt");
  double SlowFilterSG = wuReadData::ReadValue<double>("SlowFilterSG","ReadData.txt");
  int SlowFilterRange = wuReadData::ReadValue<int>("SlowFilterRange","ReadData.txt");
  double PreampTau = wuReadData::ReadValue<double>("PreampTau","ReadData.txt");
  
  off->SetPulsePolarity(PulsePolarity);
  off->SetADCMSPS(ADCMSPS);
  off->SetCalculateVertexPoint(CalculateVertexPoint);
  off->SetCalculateBaselinePoint(CalculateBaselinePoint);
  off->SetCalculateRiseTimeType(CalculateRiseTimeType);//设置上升时间计算方法
  
  off->SetFastFilterPar(FastFilterFL,FastFilterFG,FastFilterThre);//100
  off->SetSlowFilterPar(SlowFilterSL,SlowFilterSG,SlowFilterRange);
  off->SetPreampTau(PreampTau);

  off->PrintFilterPar();
  
  TCanvas *c1 = new TCanvas("c1","",1200,800);
  // gStyle->SetOptStat(0);//不显示统计框
  c1->ToggleEventStatus();//底端信息栏
  // c1->ToggleEditor();
  // c1->ToggleToolBar();
  // c1->Clear("");
  // c1->Divide(/*col*/,/*raw*/);
  // c1->SetGridx();//SetGridy();
  // c1->SetLogx();//SetLogy(); SetLogz();
  // c1->SetName("");

  TH2D *fftpsd = new TH2D("fftpsd","",5000,0,10000,5000,0,500);


  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  int wavedata[65535];
  int CountPoint = 0;
  
  fftw_complex *in;
  double *out;
  fftw1d *fft1d;
  int mhit;
  int tempenergy;
  int sized = 2048;
  for (Long64_t entry = 0; entry < TotalEntry; ++entry)
    {//循环处理从这里开始
      fChain->GetEvent(entry);
      if(entry == 0)
	{
	  in = Malloc_fftw_complex(int(size));
	  out = Malloc_fftw_real(int(size));
	  fft1d = new fftw1d(int(size),-1);
	}
      if(entry%10000==0) std::cout<<entry<<std::endl;

      if(ch != SelectChannel) continue;
      off->SetEventData(size, data);
      off->GetWaveData(wavedata);

      tempenergy = off->GetQEnergy(20,120);
      // tempenergy = off->GetQEnergyTriggerPeak(20,120);

      if(tempenergy > 0) 
	{
	  for (int i = 0; i < size; ++i)
	    {
	      in[i][0] = wavedata[i];
	      in[i][1] = 0;
	    }

	  fft1d->ForwardGetAmplitude(in,out);

	  mhit = size/2;
	  double fsum=0;
	  for (int i = 0; i < mhit; ++i)
	    {
	      fsum+=out[i];
	    }
	  fftpsd->Fill(tempenergy*0.0426472-3.81777,fsum/out[0]);
	  // fftpsd->Fill(tempenergy*0.0263754-23.2883,fsum/out[0]);
	}

    }//循环处理到这里结束

  c1->cd();
  fftpsd->Draw("colz");
  c1->Update();


  gBenchmark->Show("tree");//计时结束并输出时间
  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  // and enter the event loop...
  theApp->Run();
  delete theApp;
  
  return 0;
}
  
  

// 
// mainpsd.cc ends here
