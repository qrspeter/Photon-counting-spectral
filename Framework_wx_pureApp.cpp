/***************************************************************
 * Name:      Framework_wx_pureApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Peter ()
 * Created:   2021-11-29
 * Copyright: Peter ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "Framework_wx_pureApp.h"
#include "Framework_wx_pureMain.h"
//
//#include "acton_monocromator/acton_monochromator.h"
//#include "srs_counter/srs_counter.h"
//
//
//ARC_MONOCHROMATOR monochromator;
//// ARC_MONOCHROMATOR monochromator(5, 200, 500, 5, 1, FRONT);
//SR_COUNTER counter;

IMPLEMENT_APP(Framework_wx_pureApp); // analog of main(), also wxIMPLEMENT_APP(MyApp)


bool Framework_wx_pureApp::OnInit()
{
    Framework_wx_pureFrame* frame = new Framework_wx_pureFrame(0L, _("Framework_wx"));



    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
//    wxSize  graph_size = wxSize(100,300);
    frame->SetSize( 900, 675);
    frame->Show();

    return true;
}
