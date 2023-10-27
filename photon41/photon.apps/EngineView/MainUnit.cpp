//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

 extern "C"
{
	#pragma link "awpipl2b.lib"
	#pragma link "JPEGLIB.lib"
}
#pragma link "TinyXML.lib"
#pragma link "awplflibb.lib"

TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
	: TForm(Owner)
{
		srand (time(NULL));
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Exit1Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::LoadEngine1Click(TObject *Sender)
{
	if (OpenDialog1->Execute())
	{
		//
		AnsiString _ansi = OpenDialog1->FileName;
		if (!m_engine.Load(_ansi.c_str()))
		{
			ShowMessage("Cannot load engine.");
		}
		else
		{
			//
			this->CheckListBox1->Clear();
			ILFObjectDetector* detector = m_engine.GetDetector(0);
			if (detector == NULL)
				return;
			TLFObjectList* list = detector->GetStrongs();
			for (int i = 0; i < detector->GetStagesCount(); i++)
			{
				ILFStrong* strong = dynamic_cast<ILFStrong*>(list->Get(i));

				this->CheckListBox1->Items->Add("Stage " + IntToStr(i) + " : " + IntToStr(strong->GetCount()) + " items");
			}
			this->CheckListBox1->CheckAll(cbChecked, false, true);

			Label4->Caption = ExtractFileName(OpenDialog1->FileName);
			Label5->Caption = IntToStr(m_engine.GetDetector(0)->GetStagesCount());
			Label6->Caption = IntToStr(int(m_engine.GetDetector(0)->GetObjectType()));
			DrawEngine();
		}
	}
}

void __fastcall TForm3::DrawCube(TCanvas* cnv)
{
	TCube cube(&t);
	cube.Draw(cnv);
}

//---------------------------------------------------------------------------
void __fastcall TForm3::DrawEngine()
{
	//
	ILFObjectDetector* detector = m_engine.GetDetector(0);
	if (detector == NULL)
		return;

	Graphics::TBitmap* bmp = new Graphics::TBitmap();
	bmp->Width = PaintBox1->Width;
	bmp->Height= PaintBox1->Height;

	TCanvas* cnv = bmp->Canvas;
	TRect rect;  rect.left = 0; rect.top = 0;
	rect.Bottom = ClientHeight;
	rect.right =  ClientWidth;

	cnv->Brush->Color = clGrayText;
	cnv->FillRect(rect);
	if (this->RadioGroup1->ItemIndex == 1)
	{
	   if (CheckBox1->Checked)
			DrawCube(cnv);
	}

	if (CheckListBox2->State[0])
		DrawLayer0(cnv);
	if (CheckListBox2->State[1])
		DrawLayer1(cnv);
	if (CheckListBox2->State[2])
		DrawLayer2(cnv);
	if (CheckListBox2->State[3])
		DrawLayer3(cnv);


	PaintBox1->Canvas->Draw(0,0,bmp);
	delete bmp;

}
void __fastcall TForm3::PaintBox1Paint(TObject *Sender)
{
	this->DrawEngine();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::DrawLayer0(TCanvas* cnv)
{
	ILFObjectDetector* detector = m_engine.GetDetector(0);
	if (detector == NULL)
		return;
	int w = PaintBox1->Width;
	int h = PaintBox1->Height;
	double h_scale = h / detector->GetBaseHeight();
	double v_scale = w / detector->GetBaseWidth();
	double scale = h_scale > v_scale ? v_scale : h_scale;
	cnv->Brush->Style = bsClear;
	cnv->Brush->Color = clGray;
	cnv->Pen->Color = clGray;

	TLFObjectList* list = detector->GetStrongs();
	for (int i = 0; i < list->GetCount(); i++)
	{
		if (!this->CheckListBox1->State[i])
			continue;
		ILFStrong* strong = dynamic_cast<ILFStrong*>(list->Get(i));
		if (strong)
		{
			awpPoint cc;
			cc.X = 0;
			cc.Y = 0;
			for (int j = 0; j < strong->GetCount(); j++)
			{
				ILFWeak* weak = strong->GetWeak(j);
				if (weak != NULL)
				{
					ILFFeature* feature = weak->Fetaure();
					awpRect r = feature->GetRect();
					TLFRect rect;
					rect.SetRect(r);
					rect.Scale(scale);
					awpPoint p = rect.Center();
					cc.X += p.X;
					cc.Y += p.Y;
					TRect vcl_rect;

					int w = rect.Width() / 3;
					int h = rect.Height() / 3;
					for (int y = -1; y <= 1; y++)
					{
						for (int x = -1; x <= 1; x++)
						{
							awpPoint pp;

							double rnd = (rand() % 5 + 1) / 5.;
							pp.X = p.X + w*x*rnd;
							pp.Y = p.Y + h*y*rnd;
							vcl_rect.init(pp.X - 5, pp.Y - 5, pp.X + 5, pp.Y + 5);
							if (this->RadioGroup1->ItemIndex == 0)
							{
								cnv->Ellipse(vcl_rect);
								cnv->MoveTo(p.X, p.Y);
								cnv->LineTo(pp.X, pp.Y);
							}
							else
							{
								T3DPoint p3d;
								T3DPoint p3d1;

								p3d.x = pp.X / (scale*detector->GetBaseWidth());
								p3d.y = pp.Y / (scale*detector->GetBaseHeight());
								p3d.z = 0;

								double rnd = (rand() % 10 + 1) / 50.;

								p3d1.x = p.X / (scale*detector->GetBaseWidth());
								p3d1.y = p.Y / (scale*detector->GetBaseHeight());
								p3d1.z = 0.3;

								TPoint p1;
								TPoint p2;
								p1 = t.GetProjection(p3d);
								p2 = t.GetProjection(p3d1);

								cnv->Ellipse(p1.X - 2, p1.Y - 2, p1.X + 2, p1.Y + 2);
								cnv->MoveTo(p1.X, p1.Y);
								cnv->LineTo(p2.X, p2.Y);

							}
						}
					}
				}
			}
		}
	}

}

void __fastcall TForm3::DrawLayer1(TCanvas* cnv)
{
	ILFObjectDetector* detector = m_engine.GetDetector(0);
	if (detector == NULL)
		return;

	cnv->Brush->Style = bsClear;
	cnv->Brush->Color = clSilver;
	cnv->Pen->Color = clSilver;

	int w = PaintBox1->Width;
	int h = PaintBox1->Height;
	double h_scale = h / detector->GetBaseHeight();
	double v_scale = w / detector->GetBaseWidth();
	double scale = h_scale > v_scale ? v_scale : h_scale;
	TLFObjectList* list = detector->GetStrongs();
	for (int i = 0; i < list->GetCount(); i++)
	{
		if (!this->CheckListBox1->State[i])
			continue;

		ILFStrong* strong = dynamic_cast<ILFStrong*>(list->Get(i));
		if (strong)
		{
			awpPoint cc;
			cc.X = 0;
			cc.Y = 0;
			for (int j = 0; j < strong->GetCount(); j++)
			{
				ILFWeak* weak = strong->GetWeak(j);
				if (weak != NULL)
				{
					ILFFeature* feature = weak->Fetaure();
					awpRect r = feature->GetRect();
					TLFRect rect;
					rect.SetRect(r);
					rect.Scale(scale);
					awpPoint p = rect.Center();
					cc = p;
					if (this->RadioGroup1->ItemIndex == 0)
						cnv->Ellipse(cc.X - 10, cc.Y - 10, cc.X + 10, cc.Y+10);
					else
					{
							T3DPoint p3d;

							double rnd = (rand() % 10 + 1) / 20.;

							p3d.x = cc.X / (scale*detector->GetBaseWidth());
							p3d.y = cc.Y / (scale*detector->GetBaseHeight());
							p3d.z = 0.3;
							TPoint p1;
							p1 = t.GetProjection(p3d);

							cnv->Ellipse(p1.X - 5, p1.Y - 5, p1.X + 5, p1.Y +5);
					}

				}
			}
			cc.X /= strong->GetCount();
			cc.Y /= strong->GetCount();
		}
	}
}

void __fastcall TForm3::DrawLayer2(TCanvas* cnv)
{
	ILFObjectDetector* detector = m_engine.GetDetector(0);
	if (detector == NULL)
		return;

	cnv->Brush->Style = bsClear;
	cnv->Brush->Color = clSilver;
	cnv->Pen->Color = clSilver;

	int w = PaintBox1->Width;
	int h = PaintBox1->Height;
	double h_scale = h / detector->GetBaseHeight();
	double v_scale = w / detector->GetBaseWidth();
	double scale = h_scale > v_scale ? v_scale : h_scale;

	TLFObjectList* list = detector->GetStrongs();
	for (int i = 0; i < list->GetCount(); i++)
	{

		if (!this->CheckListBox1->State[i])
			continue;

		ILFStrong* strong = dynamic_cast<ILFStrong*>(list->Get(i));
		if (strong)
		{
			awpPoint cc;
			cc.X = 0;
			cc.Y = 0;
			for (int j = 0; j < strong->GetCount(); j++)
			{
				ILFWeak* weak = strong->GetWeak(j);
				if (weak != NULL)
				{
					ILFFeature* feature = weak->Fetaure();
					awpRect r = feature->GetRect();
					TLFRect rect;
					rect.SetRect(r);
					rect.Scale(scale);
					awpPoint p = rect.Center();
					cc.X += p.X;
					cc.Y += p.Y;
				}
			}
			cc.X /= strong->GetCount();
			cc.Y /= strong->GetCount();
			int dx = detector->GetBaseWidth() * scale/2 - cc.X;
			int dy = detector->GetBaseHeight()* scale/2 - cc.Y;
			cc.X = cc.X - 2*dx;
			cc.Y = cc.Y - 2*dy;

			for (int j = 0; j < strong->GetCount(); j++)
			{
				ILFWeak* weak = strong->GetWeak(j);
				if (weak != NULL)
				{
					ILFFeature* feature = weak->Fetaure();
					awpRect r = feature->GetRect();
					TLFRect rect;
					rect.SetRect(r);
					rect.Scale(scale);
					awpPoint p = rect.Center();
					if (RadioGroup1->ItemIndex == 0)
					{
						cnv->MoveTo(cc.X, cc.Y);
						cnv->LineTo(p.X, p.Y);
					}
					else
					{
						T3DPoint p3d;
						T3DPoint p3d1;

						p3d.x = p.X / (scale*detector->GetBaseWidth());
						p3d.y = p.Y / (scale*detector->GetBaseHeight());
						p3d.z = 0.3;

						double rnd = (rand() % 10 + 1) / 50.;

						p3d1.x = cc.X / (scale*detector->GetBaseWidth());
						p3d1.y = cc.Y / (scale*detector->GetBaseHeight());
						p3d1.z = 0.7;

						TPoint p1;
						TPoint p2;
						p1 = t.GetProjection(p3d);
						p2 = t.GetProjection(p3d1);
						cnv->MoveTo(p1.X, p1.Y);
						cnv->LineTo(p2.X, p2.Y);
					}
				}
			}
		}
	}

	for (int i = 0; i < list->GetCount(); i++)
	{
		if (!this->CheckListBox1->State[i])
			continue;


		ILFStrong* strong = dynamic_cast<ILFStrong*>(list->Get(i));
		if (strong)
		{
			awpPoint cc;
			cc.X = 0;
			cc.Y = 0;
			for (int j = 0; j < strong->GetCount(); j++)
			{
				ILFWeak* weak = strong->GetWeak(j);
				if (weak != NULL)
				{
					ILFFeature* feature = weak->Fetaure();
					awpRect r = feature->GetRect();
					TLFRect rect;
					rect.SetRect(r);
					rect.Scale(scale);
					awpPoint p = rect.Center();
					cc.X += p.X;
					cc.Y += p.Y;
				}
			}
			cc.X /= strong->GetCount();
			cc.Y /= strong->GetCount();
			int dx = detector->GetBaseWidth() * scale /2 - cc.X;
			int dy = detector->GetBaseHeight() * scale/2 - cc.Y;

			cc.X = cc.X - 2*dx;
			cc.Y = cc.Y - 2*dy;
			if (RadioGroup1->ItemIndex == 0)
			{
				cnv->Ellipse(cc.X - 10, cc.Y - 10, cc.X + 10, cc.Y+10);
			}
			else
			{
				T3DPoint p3d;

				double rnd = (rand() % 10 + 1) / 20.;

				p3d.x = cc.X / (scale*detector->GetBaseWidth());
				p3d.y = cc.Y / (scale*detector->GetBaseHeight());
				p3d.z = 0.7;
				TPoint p1;
				p1 = t.GetProjection(p3d);
				cnv->Ellipse(p1.X - 10, p1.Y - 10, p1.X + 10, p1.Y+10);
			}
		}
	}

}

void __fastcall TForm3::DrawLayer3(TCanvas* cnv)
{
	ILFObjectDetector* detector = m_engine.GetDetector(0);
	if (detector == NULL)
		return;
		TLFObjectList* list = detector->GetStrongs();
	cnv->Brush->Style = bsClear;
	cnv->Brush->Color = clSilver;
	cnv->Pen->Color = clSilver;
	cnv->Pen->Width  = 2;

	int w = PaintBox1->Width;
	int h = PaintBox1->Height;
	double h_scale = h / detector->GetBaseHeight();
	double v_scale = w / detector->GetBaseWidth();
	double scale = h_scale > v_scale ? v_scale : h_scale;

	awpPoint p;
	awpPoint center;
	awpPoint cc;
	center.X  =0;
	center.Y  =0;
	p.X = - 1000;
	p.Y = -1000;
	int count= 1;
	for (int i = 0; i < list->GetCount(); i++)
	{

		if (!this->CheckListBox1->State[i])
			continue;

		count++;
		ILFStrong* strong = dynamic_cast<ILFStrong*>(list->Get(i));
		if (strong)
		{

			cc.X = 0;
			cc.Y = 0;
			for (int j = 0; j < strong->GetCount(); j++)
			{
				ILFWeak* weak = strong->GetWeak(j);
				if (weak != NULL)
				{
					ILFFeature* feature = weak->Fetaure();
					awpRect r = feature->GetRect();
					TLFRect rect;
					rect.SetRect(r);
					rect.Scale(scale);
					awpPoint p = rect.Center();
					cc.X += p.X;
					cc.Y += p.Y;
				}
			}
			cc.X /= strong->GetCount();
			cc.Y /= strong->GetCount();
			int dx = detector->GetBaseWidth() * scale /2 - cc.X;
			int dy = detector->GetBaseHeight() * scale/2 - cc.Y;

			cc.X = cc.X - 2*dx;
			cc.Y = cc.Y - 2*dy;
			if (p.X > -100 && p.Y > -100)
			{

				if (RadioGroup1->ItemIndex == 0)
				{
					cnv->MoveTo(p.X, p.Y);
					cnv->LineTo(cc.X, cc.Y);
				}
				else
				{
						T3DPoint p3d;
						T3DPoint p3d1;

						p3d.x = p.X / (scale*detector->GetBaseWidth());
						p3d.y = p.Y / (scale*detector->GetBaseHeight());
						p3d.z = 0.7;

						double rnd = (rand() % 10 + 1) / 50.;

						p3d1.x = cc.X / (scale*detector->GetBaseWidth());
						p3d1.y = cc.Y / (scale*detector->GetBaseHeight());
						p3d1.z = 0.7;

						TPoint p1;
						TPoint p2;
						p1 = t.GetProjection(p3d);
						p2 = t.GetProjection(p3d1);
						cnv->MoveTo(p1.X, p1.Y);
						cnv->LineTo(p2.X, p2.Y);
				}
			}
			p = cc;
			center.X += p.X;
			center.Y += p.Y;
		}
	}
	center.X /= count;
	center.Y /= count;
	cnv->Pen->Color = clSilver;
	for (int i = 0; i < list->GetCount(); i++)
	{

		if (!this->CheckListBox1->State[i])
			continue;


		ILFStrong* strong = dynamic_cast<ILFStrong*>(list->Get(i));
		if (strong)
		{

			cc.X = 0;
			cc.Y = 0;
			for (int j = 0; j < strong->GetCount(); j++)
			{
				ILFWeak* weak = strong->GetWeak(j);
				if (weak != NULL)
				{
					ILFFeature* feature = weak->Fetaure();
					awpRect r = feature->GetRect();
					TLFRect rect;
					rect.SetRect(r);
					rect.Scale(scale);
					awpPoint p = rect.Center();
					cc.X += p.X;
					cc.Y += p.Y;
				}
			}
			cc.X /= strong->GetCount();
			cc.Y /= strong->GetCount();
			int dx = detector->GetBaseWidth() * scale/2 - cc.X;
			int dy = detector->GetBaseHeight() * scale/2 - cc.Y;

			cc.X = cc.X - 2*dx;
			cc.Y = cc.Y - 2*dy;
			if (p.X > -100 && p.Y > -100)
			{
				if (RadioGroup1->ItemIndex == 0)
				{
					cnv->MoveTo(center.X, center.Y);
					cnv->LineTo(cc.X, cc.Y);
				}
				else
				{
						T3DPoint p3d;
						T3DPoint p3d1;

						p3d.x = center.X / (scale*detector->GetBaseWidth());
						p3d.y = center.Y / (scale*detector->GetBaseHeight());
						p3d.z = 0.9;

						double rnd = (rand() % 10 + 1) / 50.;

						p3d1.x = cc.X / (scale*detector->GetBaseWidth());
						p3d1.y = cc.Y / (scale*detector->GetBaseHeight());
						p3d1.z = 0.7;

						TPoint p1;
						TPoint p2;
						p1 = t.GetProjection(p3d);
						p2 = t.GetProjection(p3d1);
						cnv->MoveTo(p1.X, p1.Y);
						cnv->LineTo(p2.X, p2.Y);
				}
			}
		}
	}
	cc = center;
	if (RadioGroup1->ItemIndex == 0)
		cnv->Ellipse(cc.X - 15, cc.Y - 15, cc.X + 15, cc.Y+15);
	else
	{
		T3DPoint p3d;

		p3d.x = cc.X / (scale*detector->GetBaseWidth());
		p3d.y = cc.Y / (scale*detector->GetBaseHeight());
		p3d.z = 0.9;
		TPoint p1;
		p1 = t.GetProjection(p3d);
		cnv->Ellipse(p1.X - 10, p1.Y - 10, p1.X + 10, p1.Y+10);
	}
	cnv->Pen->Width = 1;
}


void __fastcall TForm3::CheckListBox1Click(TObject *Sender)
{
	PaintBox1->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::FormShow(TObject *Sender)
{
	this->CheckListBox2->CheckAll(cbChecked, false, true);
}
//---------------------------------------------------------------------------

void __fastcall TForm3::CheckListBox2Click(TObject *Sender)
{
	PaintBox1->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::CheckListBox2ClickCheck(TObject *Sender)
{
	PaintBox1->Refresh();
}
//---------------------------------------------------------------------------


void __fastcall TForm3::RadioGroup1Click(TObject *Sender)
{
   PaintBox1->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::PaintBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	this->m_mdown = true;
	m_dx = X;
	m_dy = Y;

}
//---------------------------------------------------------------------------

void __fastcall TForm3::PaintBox1MouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
	 if (m_mdown)
	 {
		m_dx -= X;
		m_dy -= Y;
		t.ay = t.ay + m_dy;
		t.az = t.az + m_dx;
		this->DrawEngine();
		m_dx = X;
		m_dy = Y;
	 }

}
//---------------------------------------------------------------------------

void __fastcall TForm3::PaintBox1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	m_mdown = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Panel1Resize(TObject *Sender)
{
	t.sx = 0.5*PaintBox1->ClientWidth;
	t.sy = 0.5*PaintBox1->ClientHeight;
	t.zoom = 0.03;
	this->DrawEngine();

}
//---------------------------------------------------------------------------

void __fastcall TForm3::CheckBox1Click(TObject *Sender)
{
	DrawEngine();
}
//---------------------------------------------------------------------------

