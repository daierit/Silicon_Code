#include <iostream>
#include <fstream>
#include <string.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TFrame.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TROOT.h>
#include <TStyle.h>
using namespace std;

int main(int argc, char *argv[]){
  const int Num_Wafer=3;
  char *Date[Num_Wafer];
  int Wafer[Num_Wafer];
  vector<int> Volt_cut;
  //for(int i=0; i<argc;++i){
  //  cout << i << " " << argv[i] << endl;
  //}
  if(argc>6){
    Date[0] = argv[1]; Wafer[0] = atoi(argv[2]);
    Date[1] = argv[3]; Wafer[1] = atoi(argv[4]);
    Date[2] = argv[5]; Wafer[2] = atoi(argv[6]);
    for(int i=7;i<argc;++i)
      Volt_cut.push_back(atoi(argv[i]));
  }
  else{
    cout << "Please give 3 date, 3 wafer number and volt(./**** 200106 1 200107 5 200108 9 100 200...)" << endl;
    return 0;
  }

  gStyle-> SetOptStat(0);
  gStyle-> SetTitleX(0.15);
  gStyle-> SetTitleY(0.97);
  TCanvas* c0 = new TCanvas("c0","",1300,900);
  c0-> cd();
  //c0-> SetLogy();
  TPad *pad1 = new TPad("pad1","",0,0.5,0.4,1);
  TPad *pad2 = new TPad("pad2","",0.4,0.5,0.8,1);
  TPad *pad3 = new TPad("pad3","",0,0,0.4,0.5);
  TPad *pad4 = new TPad("pad4","",0.4,0,0.8,0.5);
  TPad *pad5 = new TPad("pad5","",0.77,0,1,1);
  pad1-> SetGridx();
  pad1-> SetGridy();
  pad2-> SetGridx();
  pad2-> SetGridy();
  pad3-> SetGridx();
  pad3-> SetGridy();
  pad4-> SetGridx();
  pad4-> SetGridy();
  pad5-> GetFrame()-> SetBorderSize(-1);
  pad5-> GetFrame()-> SetFillColor(0);
  pad1-> Draw();
  pad2-> Draw();
  pad3-> Draw();
  pad4-> Draw();
  pad5-> Draw();
  pad1-> SetLogy();
  pad2-> SetLogy();
  pad3-> SetLogy();
  pad4-> SetLogy();
  double dlimit=8;
  double ulimit=2000;

  /********load channel width group**************/
  int width=-1;
  string rmread;
  vector<vector<int>> chgrp;
  fstream rm;
  rm.open("Channel_widthmap.txt",ios::in);
  if(rm){
    chgrp.resize(4);
    while(rm >> rmread){
      if(rmread[0] == 'W')
        width+=1;
      else
        chgrp[width].push_back(atoi(rmread.c_str()));
    }
  }
  else{
    cout << "can't open Channel map" << endl;
    return 0;
  }
  /**********************************************/

  fstream file;
  double V,I;
  TGraph *gr[3][4];
  TGraph *go[3][4];
  TMultiGraph *mg1, *mg2, *mg3, *mg4;
  TLegend *leg1, *leg2;
  int Color[3]={2,3,4};
  int Style[8]={39,22,34,20,37,26,28,24};
  double Size[4]={1.5,1.5,1.5,1};
  char path[200], name[200];
  for(int ivc=0;ivc<Volt_cut.size();++ivc){
    mg1 = new TMultiGraph();
    mg2 = new TMultiGraph();
    mg3 = new TMultiGraph();
    mg4 = new TMultiGraph();
    leg1 = new TLegend(0,0.7,0.95,0.9,NULL,"brNDC");
    leg2 = new TLegend(0,0.3,0.95,0.65,NULL,"brNDC");
    for(int wafer=0;wafer<Num_Wafer;++wafer){
      for(int ig=0;ig<4;++ig){
        gr[wafer][ig] = new TGraph();
        go[wafer][ig] = new TGraph();
        for(int ich=0;ich<chgrp[ig].size();++ich){
          sprintf(path,"/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s_NCU/ProbeCard/HGcal_6inch_%02d/%03d/IV.txt",Date[wafer],Wafer[wafer],chgrp[ig][ich]);
          file.open(path,ios::in);
          if(file){
            while(file >> V >> I)
              //cout << V << "\t" << I << endl;
              if(V == Volt_cut[ivc]){
                gr[wafer][ig]-> SetPoint(ich+1,chgrp[ig][ich],I);
                go[wafer][ig]-> RemovePoint(ich+1);
                break;
              }
              else
                if(I<200)
                  go[wafer][ig]-> SetPoint(ich+1,chgrp[ig][ich],10);
                else
                  gr[wafer][ig]-> SetPoint(ich+1,chgrp[ig][ich],2000);
          }
          else{
            cout << "cna't open " << path << endl;
          }
          file.close();
        }//end ich loop
        gr[wafer][ig]-> SetMarkerColor(Color[wafer]);
        go[wafer][ig]-> SetMarkerColor(Color[wafer]);
        gr[wafer][ig]-> SetMarkerStyle(Style[ig]);
        go[wafer][ig]-> SetMarkerStyle(Style[ig+4]);
        gr[wafer][ig]-> SetMarkerSize(Size[ig]);
        go[wafer][ig]-> SetMarkerSize(Size[ig]);
        if(ig==0){
          mg3-> Add(gr[wafer][ig]);
          mg3-> Add(go[wafer][ig]);
        }
        else if(ig==1){
          mg4-> Add(gr[wafer][ig]);
          mg4-> Add(go[wafer][ig]);
        }
        else if(ig==2){
          mg1-> Add(gr[wafer][ig]);
          mg1-> Add(go[wafer][ig]);
        }
        else if(ig==3){
          mg2-> Add(gr[wafer][ig]);
          mg2-> Add(go[wafer][ig]);
        }
      }//end ig loop
      if(wafer==0){
        leg2-> AddEntry(gr[0][0],"22#mum","p");
        leg2-> AddEntry(gr[0][1],"42#mum","p");
        leg2-> AddEntry(gr[0][2],"62#mum","p");
        leg2-> AddEntry(gr[0][3],"82#mum","p");
        leg2-> AddEntry(go[0][3],Form("#splitline{Reach limit}{before %dV}",Volt_cut[ivc]),"p");
      }
    }//end wafer loop
    leg1-> AddEntry(gr[0][3],Form("#%02d SiO_{2}/SiN_{x}",Wafer[0]),"p");
    leg1-> AddEntry(gr[1][3],Form("#%02d SiO_{2}/SiN_{x} + SU8",Wafer[1]),"p");
    leg1-> AddEntry(gr[2][3],Form("#%02d SU8",Wafer[2]),"p");
    mg1-> SetTitle(Form("%dV 62#um",Volt_cut[ivc]));
    mg1-> GetXaxis()-> SetTitle("Channel");
    mg1-> GetYaxis()-> SetTitle("nA");
    mg1-> GetYaxis()-> SetRangeUser(dlimit,ulimit);
    mg1-> GetXaxis()-> SetLimits(1,128);
    pad1-> cd();
    mg1-> Draw("ap");
    mg1-> Draw("p");
    mg2-> SetTitle(Form("%dV 82#um",Volt_cut[ivc]));
    mg2-> GetXaxis()-> SetTitle("Channel");
    mg2-> GetYaxis()-> SetTitle("nA");
    mg2-> GetYaxis()-> SetRangeUser(dlimit,ulimit);
    mg2-> GetXaxis()-> SetLimits(1,128);
    pad2-> cd();
    mg2-> Draw("ap");
    mg2-> Draw("p");
    mg3-> SetTitle(Form("%dV 22#um",Volt_cut[ivc]));
    mg3-> GetXaxis()-> SetTitle("Channel");
    mg3-> GetYaxis()-> SetTitle("nA");
    mg3-> GetYaxis()-> SetRangeUser(dlimit,ulimit);
    mg3-> GetXaxis()-> SetLimits(1,128);
    pad3-> cd();
    mg3-> Draw("ap");
    mg3-> Draw("p");
    mg4-> SetTitle(Form("%dV 42#um",Volt_cut[ivc]));
    mg4-> GetXaxis()-> SetTitle("Channel");
    mg4-> GetYaxis()-> SetTitle("nA");
    mg4-> GetYaxis()-> SetRangeUser(dlimit,ulimit);
    mg4-> GetXaxis()-> SetLimits(1,128);
    pad4-> cd();
    mg4-> Draw("ap");
    mg4-> Draw("p");
    pad5-> cd();
    leg1-> SetTextSize(0.07);
    leg2-> SetTextSize(0.07);
    leg1-> Draw();
    leg2-> Draw();
    c0-> Print(Form("/home/kycheng/Silicon_plot/FullWafer/HGcalRun2_Current_%02d_%02d_%02d_%d.png",Wafer[0],Wafer[1],Wafer[2],Volt_cut[ivc]));
  }//end V_cut loop
  return 0;
}
