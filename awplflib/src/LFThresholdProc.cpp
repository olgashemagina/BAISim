#include "_LF.h"

TLFThresholdProc::TLFThresholdProc ()
{
	TLFParameter* param = new TLFParameter();
	/* параметр: порог бинаризации изображения 
	*/
	param->SetPName("Threshold");
	param->SetValue(128.f);
	param->SetMinValue(0.f);
	param->SetMaxValue(255.f);
	this->m_Params.Add(param);
	param = new TLFParameter();
	param->SetPName("Invert");
	param->SetValue(0.f);
	param->SetMinValue(0);
	param->SetMaxValue(1);
	this->m_Params.Add(param);
	awpRect rect;
	memset(&rect, 0, sizeof(awpRect));
	m_ImageRoi.SetRect(rect);
}
TLFThresholdProc::~TLFThresholdProc ()
{

}
/* реализация алгоритма пороговой обработки изображения
*/
bool TLFThresholdProc::Process()
{
	double thr = ((TLFParameter*)this->m_Params.Get(0))->GetValue();
	double invert = ((TLFParameter*)this->m_Params.Get(1))->GetValue();
	awpImage* src = m_SourceImage.GetImage();
	if (src == NULL)
		return false;
	/* проверим исходное изображение на допустимость входных значений
	*/
	if (src->dwType != AWP_BYTE)
		return false;

	m_ResultImage.FreeImages();

	awpImage* dst = NULL;
	if (awpCopyImage(src, &dst) != AWP_OK)
		return false;
	if (dst->bChannels == 3)
	{
		if (awpConvert(dst, AWP_CONVERT_3TO1_BYTE) != AWP_OK)
		{
			_AWP_SAFE_RELEASE_(dst)
			return false;
		}
	}


	/* для выполнение преобразования воспользуемся функцией
	   из библиотеки awpipl2
	*/
	AWPBYTE* pix_dst = (AWPBYTE*)dst->pPixels;

	int sx, sy, ex, ey;
	if (this->m_ImageRoi.Width() > 8 && m_ImageRoi.Height() > 8)
	{
		sx = m_ImageRoi.Left();
		sy = m_ImageRoi.Top();
		ex = sx + m_ImageRoi.Width();
		ey = sy + m_ImageRoi.Height();
	}
	else
	 {
		sx = 0;
		sy = 0;
		ex = dst->sSizeX;
		ey = dst->sSizeY;
	 }

	for (int i = 0; i < src->sSizeY; i++)
	{
		for (int j  = 0; j < src->sSizeX; j++)
		{
			if (i >= sy && i <= ey && j >= sx && j <= ex)
			{
				if (invert != 0)
				  pix_dst[i*src->sSizeX + j] = 255 - pix_dst[i*src->sSizeX + j];
				if (pix_dst[i*src->sSizeX + j] > thr)
					pix_dst[i*src->sSizeX + j] = 255;
				else
					pix_dst[i*src->sSizeX + j] = 0;
			}
			else
					pix_dst[i*src->sSizeX + j] = 0;
		}
	}
	m_ResultImage.SetImage(dst);
	_AWP_SAFE_RELEASE_(dst)
	return true;
}

bool TLFThresholdProc::ProcessMoments(TLFRect& rect, SLFBinaryMoments& moments)
{
   memset(&moments, 0, sizeof(SLFBinaryMoments));

	if (this->m_ResultImage.GetImage() == NULL)
		return false;
	if (rect.Width() < 1 || rect.Height() < 1)
		return false;
	awpRect r = rect.GetRect();
	awpImage* img = m_ResultImage.GetImage();
	if (awpRectInImage(img, &r) != AWP_OK)
        {
        	if (r.left < 0)r.left  =0;
                if (r.right >= img->sSizeX) r.right = img->sSizeX-1;
        	if (r.top < 0)r.top  =0;
                if (r.bottom >= img->sSizeY) r.bottom = img->sSizeY-1;
 //               if (r.left >= r.right || r.top >= r.bottom)
//                	return false;
        }
	memset(&moments, 0, sizeof(SLFBinaryMoments));
	AWPBYTE* pix = (AWPBYTE*)img->pPixels;

	for (int i = r.top; i < r.bottom; i++)
	{
		for (int j = r.left; j < r.right; j++)
		{
			moments.area += (pix[i*img->sSizeX + j] > 0 ? 1:0);
			moments.cx   += (j*pix[i*img->sSizeX + j]> 0 ? j:0);
			moments.cy   += (i*pix[i*img->sSizeX + j]> 0 ? i:0);
		}
	}

	if (moments.area > 0)
	{
		moments.cx  /= moments.area;
		moments.cy  /= moments.area;

		for (int i = r.top; i < r.bottom; i++)
		{
			for (int j = r.left; j < r.right; j++)
			{
				moments.mxx   += (pix[i*img->sSizeX + j]> 0 ? (j - moments.cx)*(j - moments.cx):0);
				moments.mxy   += (pix[i*img->sSizeX + j]> 0 ? (i - moments.cy)*(j - moments.cx):0);
				moments.myy   += (pix[i*img->sSizeX + j]> 0 ? (i - moments.cy)*(i - moments.cy):0);
			}
		}

		moments.mxx  /= moments.area;
		moments.myy  /= moments.area;
	   //	moments.mxy  /= moments.area;
        moments.mxy = 0;
		if (moments.mxy == 0 && moments.myy <= moments.mxx)
		{
			moments.angle = -AWP_PI / 2;
			moments.major = 4*sqrt(moments.mxx);
			moments.minor = 4*sqrt(moments.myy);

			moments.minor_p1.X = moments.cx;
			moments.minor_p1.Y = moments.cy - moments.minor / 2;
			moments.minor_p2.X = moments.cx;
			moments.minor_p2.Y = moments.cy + moments.minor / 2;

			moments.major_p1.X = moments.cx - moments.major /2;
			moments.major_p1.Y = moments.cy;
			moments.major_p2.X = moments.cx + moments.major /2;
			moments.major_p2.Y = moments.cy;

		}
		else if (moments.mxy == 0 && moments.myy > moments.mxx)
		{
			moments.angle = 0;
			moments.major = 4*sqrt(moments.myy);
			moments.minor = 4*sqrt(moments.mxx);

			moments.minor_p1.X = moments.cx - moments.minor /2;
			moments.minor_p1.Y = moments.cy;
			moments.minor_p2.X = moments.cx + moments.minor /2;
			moments.minor_p2.Y = moments.cy;

			moments.major_p1.X = moments.cx;
			moments.major_p1.Y  = moments.cy - moments.major / 2;
			moments.major_p2.X = moments.cx;
			moments.major_p2.Y  = moments.cy + moments.major / 2;
		}
		else if (moments.myy <= moments.mxx)
		{
		   moments.angle = atan(-2*moments.mxy*moments.myy-moments.mxx + sqrt((moments.myy - moments.mxx)*(moments.myy - moments.mxx) + 4*moments.mxy*moments.mxy));
		   moments.major = sqrt(8*(moments.myy + moments.mxx + sqrt((moments.myy - moments.mxx)*(moments.myy - moments.mxx) + 4*moments.mxy*moments.mxy)));
		   moments.minor = sqrt(8*(moments.myy + moments.mxx - sqrt((moments.myy - moments.mxx)*(moments.myy - moments.mxx) + 4*moments.mxy*moments.mxy)));

		   moments.minor_p1.X = moments.cx + 0.5*moments.minor*cos(moments.angle);
		   moments.minor_p1.Y = moments.cy + 0.5*moments.minor*sin(moments.angle);
		   moments.minor_p2.X = moments.cx - 0.5*moments.minor*cos(moments.angle);
		   moments.minor_p2.Y = moments.cy - 0.5*moments.minor*sin(moments.angle);

		   moments.major_p1.X = moments.cx + 0.5*moments.major*cos(-AWP_PI /2. + moments.angle);
		   moments.major_p1.Y = moments.cy + 0.5*moments.major*sin(-AWP_PI /2. + moments.angle);
		   moments.major_p2.X = moments.cx - 0.5*moments.major*cos(-AWP_PI /2. + moments.angle);
		   moments.major_p2.Y = moments.cy - 0.5*moments.major*sin(-AWP_PI /2. + moments.angle);

		}
		else if (moments.myy > moments.mxx)
		{
		   moments.angle = atan(sqrt(moments.mxx + moments.myy + sqrt((moments.mxx - moments.myy)*(moments.mxx - moments.myy) + 4*moments.mxy*moments.mxy)) / (-2*moments.mxy + 0.000001));
		   moments.major = sqrt(8*(moments.myy + moments.mxx + sqrt((moments.myy - moments.mxx)*(moments.myy - moments.mxx) + 4*moments.mxy*moments.mxy)));
		   moments.minor = sqrt(8*(moments.myy + moments.mxx - sqrt((moments.myy - moments.mxx)*(moments.myy - moments.mxx) + 4*moments.mxy*moments.mxy)));


		   moments.minor_p1.X = moments.cx + 0.5*moments.minor*cos(moments.angle);
		   moments.minor_p1.Y = moments.cy + 0.5*moments.minor*sin(moments.angle);
		   moments.minor_p2.X = moments.cx - 0.5*moments.minor*cos(moments.angle);
		   moments.minor_p2.Y = moments.cy - 0.5*moments.minor*sin(moments.angle);

		   moments.major_p1.X = moments.cx + 0.5*moments.major*cos(-AWP_PI /2. + moments.angle);
		   moments.major_p1.Y = moments.cy + 0.5*moments.major*sin(-AWP_PI /2. + moments.angle);
		   moments.major_p2.X = moments.cx - 0.5*moments.major*cos(-AWP_PI /2. + moments.angle);
		   moments.major_p2.Y = moments.cy - 0.5*moments.major*sin(-AWP_PI /2. + moments.angle);
		}





//		awpDrawEllipse(img, p, 100, 50, moments.angle*180/AWP_PI, 0, 255, 2);
	}
	return true;
}

double GetMinMomnetsX(SLFBinaryMoments& m)
{
	double a =AWP_MIN(m.minor_p1.X, m.major_p1.X);
	double b =AWP_MIN(m.minor_p2.X, m.major_p2.X);
	return AWP_MIN(a , b);
}
double GetMaxMomnetsX(SLFBinaryMoments& m)
{
	double a =AWP_MAX(m.minor_p1.X, m.major_p1.X);
	double b =AWP_MAX(m.minor_p2.X, m.major_p2.X);
	return AWP_MAX(a , b);
}
double GetMinMomnetsY(SLFBinaryMoments& m)
{
	double a =AWP_MIN(m.minor_p1.Y, m.major_p1.Y);
	double b =AWP_MIN(m.minor_p2.Y, m.major_p2.Y);
	return AWP_MIN(a , b);
}
double GetMaxMomnetsY(SLFBinaryMoments& m)
{
	double a =AWP_MAX(m.minor_p1.Y, m.major_p1.Y);
	double b =AWP_MAX(m.minor_p2.Y, m.major_p2.Y);
	return AWP_MAX(a , b);
}

