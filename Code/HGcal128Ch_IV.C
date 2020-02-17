#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TAxis.h>
#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char*argv[]){
  char *Date;
  int Wafer;
  for(int i=0; i<argc;++i){
    cout << i << " " << argv[i] << endl;
  }
  if(argc==3){
    Date = argv[1];
    Wafer = atoi(argv[2]);
  }
  else{
    cout << "Please give date and wafer number(./HGcal128Ch_IV 200106 1)" << endl;
    return 0;
  }
  TCanvas* c1 = new TCanvas("c1","c1",1600,1200);
  c1-> Divide(2,2);
  TGraph* strip[133];
  int width=-1;
  int Style[4]={20,21,29,34};
  int Color[8]={600,693,616,607,632,634,416,419};
  bool FileCheck[133] = {true};
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
  int ist=0;
  for(int ig=0;ig<4;++ig){
    for(int ich=0;ich<chgrp[ig].size();++ich){
      double VV, II;
      vector<double> V;
      vector<double> I;
      fstream r1;
      r1.open(Form("/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s_NCU/ProbeCard/HGcal_6inch_%02d/%03d/IV.txt",Date,Wafer,chgrp[ig][ich]),ios::in);
      //r1.open(Form("/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s_NCU/KEI_2410/HGcal_6inch_%02d_Full/%03d/IV.txt",Date,Wafer,chgrp[ig][ich]),ios::in);
      if(r1){
        FileCheck[ist] = true;
        V.clear();
        I.clear();
        while(r1>>VV>>II){
          V.push_back(VV);
          I.push_back(II);
        }
        strip[ist] = new TGraph(V.size(),V.data(),I.data());
        //strip[ist]-> SetDirectory(0);
      }
      else{
        cout << "cant open " << ist << endl;
        FileCheck[ist] = false;
      }//read file
      r1.close();
      ++ist;
    }//ist
  }//g
  TMultiGraph* mg[4];
  char *width_name[4]={"22um","42um","62um","82um"};
  TLegend *leg[4];
  ist=0;
  for(int ig=0;ig<4;++ig){
    mg[ig] = new TMultiGraph();
    leg[ig] = new TLegend(0.5,0.55,0.8,0.95,NULL,"brNDC");
    leg[ig]-> SetNColumns(4);
    for(int ich=0;ich<chgrp[ig].size();++ich){
      if(FileCheck[ist]){
        strip[ist]-> SetMarkerStyle(Style[int(ich/8)]);
        strip[ist]-> SetMarkerColor(Color[int(ich%8)]);
        mg[ig]-> Add(strip[ist]);
        leg[ig]-> AddEntry(strip[ist],Form("Ch%03d",chgrp[ig][ich]),"p");
      }
      ++ist;
    }
    c1-> cd(ig+1);
    mg[ig]-> SetTitle(width_name[ig]);
    mg[ig]-> GetXaxis()-> SetTitle("V");
    mg[ig]-> GetYaxis()-> SetTitle("nA");
    mg[ig]-> GetYaxis()-> SetRangeUser(0,2000);
    mg[ig]-> GetXaxis()-> SetLimits(0,700);
    mg[ig]-> Draw("ap");
    leg[ig]-> Draw();
  }
  c1-> Print(Form("/home/kycheng/Silicon_plot/FullWafer/HGcal%02d_IV.png",Wafer));
  return 0;
}
