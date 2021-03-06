{
//=========Macro generated from canvas: Canvas_1/Canvas_1
//=========  (Thu May 18 12:57:54 2017) by ROOT version5.34/04
   TCanvas *Canvas_1 = new TCanvas("Canvas_1", "Canvas_1",664,107,363,323);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   Canvas_1->Range(59486.62,-169.3586,61179.49,1087.046);
   Canvas_1->SetFillColor(0);
   Canvas_1->SetBorderMode(0);
   Canvas_1->SetBorderSize(2);
   Canvas_1->SetLeftMargin(0.1142322);
   Canvas_1->SetRightMargin(0.08239701);
   Canvas_1->SetBottomMargin(0.1347962);
   Canvas_1->SetFrameBorderMode(0);
   Canvas_1->SetFrameBorderMode(0);
   
   TH1F *time = new TH1F("time","fwav[0].time {fwav[0].evtno<8000&&fwav[0].npeaks==1}",100,58000,62000);
   time->SetBinContent(0,66);
   time->SetBinContent(2,1);
   time->SetBinContent(3,1);
   time->SetBinContent(4,1);
   time->SetBinContent(5,2);
   time->SetBinContent(6,2);
   time->SetBinContent(8,1);
   time->SetBinContent(9,1);
   time->SetBinContent(11,2);
   time->SetBinContent(12,3);
   time->SetBinContent(13,3);
   time->SetBinContent(14,3);
   time->SetBinContent(15,2);
   time->SetBinContent(16,3);
   time->SetBinContent(17,3);
   time->SetBinContent(18,3);
   time->SetBinContent(19,3);
   time->SetBinContent(20,1);
   time->SetBinContent(21,1);
   time->SetBinContent(22,4);
   time->SetBinContent(24,4);
   time->SetBinContent(25,1);
   time->SetBinContent(26,1);
   time->SetBinContent(27,5);
   time->SetBinContent(29,13);
   time->SetBinContent(32,9);
   time->SetBinContent(34,6);
   time->SetBinContent(37,10);
   time->SetBinContent(39,8);
   time->SetBinContent(42,1);
   time->SetBinContent(44,1);
   time->SetBinContent(45,1);
   time->SetBinContent(48,2);
   time->SetBinContent(49,4);
   time->SetBinContent(50,6);
   time->SetBinContent(51,11);
   time->SetBinContent(52,29);
   time->SetBinContent(53,80);
   time->SetBinContent(54,156);
   time->SetBinContent(55,317);
   time->SetBinContent(56,453);
   time->SetBinContent(57,699);
   time->SetBinContent(58,856);
   time->SetBinContent(59,919);
   time->SetBinContent(60,853);
   time->SetBinContent(61,684);
   time->SetBinContent(62,475);
   time->SetBinContent(63,313);
   time->SetBinContent(64,173);
   time->SetBinContent(65,99);
   time->SetBinContent(66,44);
   time->SetBinContent(67,32);
   time->SetBinContent(68,16);
   time->SetBinContent(69,13);
   time->SetBinContent(70,6);
   time->SetBinContent(71,4);
   time->SetBinContent(72,2);
   time->SetBinContent(73,2);
   time->SetBinContent(76,2);
   time->SetBinContent(100,1);
   time->SetBinContent(101,2);
   time->SetEntries(6419);
   
   TF1 *PrevFitTMP = new TF1("PrevFitTMP","gaus",59680,61040);
   PrevFitTMP->SetFillColor(19);
   PrevFitTMP->SetFillStyle(0);
   PrevFitTMP->SetLineColor(2);
   PrevFitTMP->SetLineWidth(2);
   PrevFitTMP->SetChisquare(60.82942);
   PrevFitTMP->SetNDF(26);
   PrevFitTMP->GetXaxis()->SetLabelFont(42);
   PrevFitTMP->GetXaxis()->SetLabelSize(0.035);
   PrevFitTMP->GetXaxis()->SetTitleSize(0.035);
   PrevFitTMP->GetXaxis()->SetTitleFont(42);
   PrevFitTMP->GetYaxis()->SetLabelFont(42);
   PrevFitTMP->GetYaxis()->SetLabelSize(0.035);
   PrevFitTMP->GetYaxis()->SetTitleSize(0.035);
   PrevFitTMP->GetYaxis()->SetTitleFont(42);
   PrevFitTMP->SetParameter(0,890.237);
   PrevFitTMP->SetParError(0,14.51908);
   PrevFitTMP->SetParLimits(0,0,0);
   PrevFitTMP->SetParameter(1,60342.96);
   PrevFitTMP->SetParError(1,1.424191);
   PrevFitTMP->SetParLimits(1,0,0);
   PrevFitTMP->SetParameter(2,110.9796);
   PrevFitTMP->SetParError(2,1.115196);
   PrevFitTMP->SetParLimits(2,0,1291.137);
   time->GetListOfFunctions()->Add(PrevFitTMP);
   
   TPaveStats *ptstats = new TPaveStats(0.5786517,0.6677116,0.906367,0.8808777,"brNDC");
   ptstats->SetName("stats");
   ptstats->SetBorderSize(1);
   ptstats->SetFillColor(0);
   ptstats->SetTextAlign(12);
   ptstats->SetTextFont(42);
   TText *text = ptstats->AddText("#chi^{2} / ndf = 60.83 / 26");
   text = ptstats->AddText("Constant = 890.2 #pm 14.5 ");
   text = ptstats->AddText("Mean     = 6.034e+04 #pm 1.424e+00 ");
   text = ptstats->AddText("Sigma    =   111 #pm 1.1 ");
   ptstats->SetOptStat(0);
   ptstats->SetOptFit(111);
   ptstats->Draw();
   time->GetListOfFunctions()->Add(ptstats);
   ptstats->SetParent(time);

   Int_t ci;   // for color index setting
   ci = TColor::GetColor("#000099");
   time->SetLineColor(ci);
   time->GetXaxis()->SetTitle("time (psec)");
   time->GetXaxis()->SetRange(43,76);
   time->GetXaxis()->CenterTitle(true);
   time->GetXaxis()->SetNdivisions(506);
   time->GetXaxis()->SetLabelFont(42);
   time->GetXaxis()->SetLabelSize(0.05);
   time->GetXaxis()->SetTitleSize(0.06);
   time->GetXaxis()->SetTitleFont(42);
   time->GetYaxis()->SetLabelFont(42);
   time->GetYaxis()->SetLabelSize(0.05);
   time->GetYaxis()->SetTitleSize(0.035);
   time->GetYaxis()->SetTitleFont(42);
   time->GetZaxis()->SetLabelFont(42);
   time->GetZaxis()->SetLabelSize(0.035);
   time->GetZaxis()->SetTitleSize(0.035);
   time->GetZaxis()->SetTitleFont(42);
   time->Draw("");
   Canvas_1->Modified();
   Canvas_1->cd();
   Canvas_1->SetSelected(Canvas_1);
}
