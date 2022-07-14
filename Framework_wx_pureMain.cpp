/***************************************************************
 * Name:      Framework_wx_pureMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Peter ()
 * Created:   2021-11-29
 * Copyright: Peter ()
 * License:
 **************************************************************/

 // https://docs.wxwidgets.org/3.0/index.html
 // https://zetcode.com/gui/wxwidgets/
// http://doc.crossplatform.ru/wxwidgets/2.8.9/wx_wxstaticboxsizer.html


#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "Framework_wx_pureMain.h"

#include "mathplot/mathplot.h"
#include "mathplot/MathPlotConfig.h"

#include <wx/filename.h>

#include <vector>
#include <fstream>
#include <deque>
#include <algorithm> // for copy deque to vector

bool compLess(int a, int b) // for minmax_element
{
    return (a < b);
}

/*
// http://stackoverflow.com/questions/38699268/
class AcceptVectors
{
public:
    AcceptVectors(std::vector<double> arg);
};
*/
/*
#include <queue>

// http://stackoverflow.com/questions/38699268/
template<typename T>
struct my_queue : std::queue <T, std::vector<T>>
{
    using std::queue <T, std::vector<T>>::queue;
    std::vector<T>& to_vector(){return this -> c; }
};
*/

#include <unistd.h> // usleep(TIME_TO_SLEEP); ms
// sleep_for() определяется стандартом C++11, а Sleep() определяется стандартом Windows API
// https://coderoad.ru/53072750/
// https://forums.wxwidgets.org/viewtopic.php?t=36579 usleep is no worse than wxStopWatch
// wxStopWatch is the *only* way under MSW for accurately measuring small time intervals, because it uses the PerformanceCounter API. All other methods work with a resolution of ~15ms

//#include <future> // for async
//#include <thread> // for thread
 // запуcкается, но без толку
 //   auto tl = std::thread([&monochromator, wavelength]{return monochromator.GoToWavelength(wavelength);});

  //  x = monochromator.GoToWavelength(monochromator.GetWavelengthStart());

// добавить считывание решеток из монохроматора и вывод названий в виде сторок в списке.
// возможно не стоит менять решетку при открытии... хз. Или при смене длины волны опрашивать монохроматор о готовности, чтобы не засыпать командами?
// Например если перемещение более чем на 20 нм.
// для настроек создать структуру с данными того же формата и записывать туда при загрузке-выгрузке.
// при подключении монохроматора может и считывать структуру с решетками (построчно), а не тестировать на ввод? Хотя одно другому не помешает.
// как проводить прерывание цикла? Ранее было сделано через тайм в void CChildView::MonochromatorStop() убивание цикла и сброс данных
// можно через опрос состояния кнопки, было нажатие или нет... Например добавить переменную, в котору записывать клики при нажатии если он состоялось во время записи.
// и переспрашивать "прервать?" чтобы возобновить, если что.




#include "acton_monocromator/acton_monochromator.h"
#include "srs_counter/srs_counter.h"

extern ARC_MONOCHROMATOR monochromator;
extern SR_COUNTER counter;

const wxString Ports[] = { wxT("COM1"), wxT("COM2"), wxT("COM3"), wxT("COM4"), wxT("COM5"), wxT("COM6"), wxT("COM7"), wxT("COM8"), wxT("COM9")};
const wxString grates[] = { wxT("300"), wxT("150")}; // rewrite into a function later
const wxString slopes[] = { wxT("Rise"), wxT("Fall")}; // rewrite into a function later

bool measurementStop = FALSE;

//mpWindow    framework_graph;
//mpFXYVector frameworkVector;



std::vector <double> vWavelength; //{ 401, 450, 500, 550, 600, 650, 700};
std::vector <double> vAveragedCounts; //{ 0, 10, 45, 90, 45, 10, 0};


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(Framework_wx_pureFrame, wxFrame)
    EVT_CLOSE(Framework_wx_pureFrame::OnClose)
    EVT_MENU(idMenuSaveAs, Framework_wx_pureFrame::OnSaveAs)
    EVT_MENU(idMenuAbout, Framework_wx_pureFrame::OnAbout)
    EVT_MENU(idMenuQuit, Framework_wx_pureFrame::OnQuit)

    EVT_CHOICE(idCounterCOM, Framework_wx_pureFrame::CounterCOM) // https://docs.wxwidgets.org/3.0/classwx_choice.html
    EVT_BUTTON(idCounterConnect, Framework_wx_pureFrame::CounterConnect)
    EVT_CHOICE(idCounterSlope, Framework_wx_pureFrame::CounterSlope)
    EVT_TEXT_ENTER(idCounterDiscr, Framework_wx_pureFrame::CounterDiscr)
    EVT_TEXT_ENTER(idCounterSampleTime, Framework_wx_pureFrame::CounterSampleTime)
    EVT_TEXT_ENTER(idCounterDeadTime, Framework_wx_pureFrame::CounterDeadTime)

    EVT_BUTTON(idMonoConnect, Framework_wx_pureFrame::MonoConnect)
    EVT_CHOICE(idMonoCOM, Framework_wx_pureFrame::MonoCOM) // https://docs.wxwidgets.org/3.0/classwx_choice.html
    EVT_CHOICE(idMonoGrate, Framework_wx_pureFrame::MonoGrate) // https://docs.wxwidgets.org/3.0/classwx_choice.html
  //  EVT_TEXT(idMonoWavelengthStart, Framework_wx_pureFrame::MonoWavelengthStart)
    EVT_TEXT_ENTER(idMonoWavelengthStart, Framework_wx_pureFrame::MonoWavelengthStart) // https://docs.wxwidgets.org/3.0/classwx_spin_ctrl.html
//    EVT_SPINCTRL(idMonoWavelengthStart, Framework_wx_pureFrame::MonoWavelengthStart) // invalid static_cast from.... возможно из-за этого не обрабатывается ENTER, т.к. он обрабатывается тут
   // EVT_TEXT(idMonoWavelengthStop, Framework_wx_pureFrame::MonoWavelengthStop)
    EVT_TEXT_ENTER(idMonoWavelengthStop, Framework_wx_pureFrame::MonoWavelengthStop)
//    EVT_SPINCTRL(idMonoWavelengthStop, Framework_wx_pureFrame::MonoWavelengthStop) // invalid static_cast from....
//    EVT_TEXT(idMonoWavelengthStep, Framework_wx_pureFrame::MonoWavelengthStep)
    EVT_TEXT_ENTER(idMonoWavelengthStep, Framework_wx_pureFrame::MonoWavelengthStep)
    // EVT_LISTBOX_DCLICK
    EVT_BUTTON(idMonoStart, Framework_wx_pureFrame::MonoStart)
    EVT_BUTTON(idMonoStop, Framework_wx_pureFrame::MonoStop)


    EVT_BUTTON(idAdjustmentStart, Framework_wx_pureFrame::AdjustmentStart)
    EVT_BUTTON(idAdjustmentStop, Framework_wx_pureFrame::AdjustmentStop)
END_EVENT_TABLE()

Framework_wx_pureFrame::Framework_wx_pureFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
// Peter. Menu described there https://docs.wxwidgets.org/3.0/overview_helloworld.html
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuSaveAs, _("&Save as...\tCtrl-s"), _("Save data"));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* settingMenu = new wxMenu(_T(""));
    settingMenu->Append(idMenuSettingSave, _("&Save setting\tCtrl-p"), _("Save current setting"));
    settingMenu->Append(idMenuSettingDefault, _("&Load default\tCtrl-d"), _("Load default setting"));
    mbar->Append(settingMenu, _("&Setting"));


    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR



//    ARC_MONOCHROMATOR monochromator;

    wxPanel *framework_panel = new wxPanel(this, -1);

    wxBoxSizer *framework_sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *control_sizer   = new wxBoxSizer( wxVERTICAL );

// http://doc.crossplatform.ru/wxwidgets/2.8.9/wx_wxstaticboxsizer.html
//    wxPanel *wxConnecting = new wxPanel(framework_panel, -1, wxDefaultPosition, wxSize(-1, -1), wxBORDER_SUNKEN, wxT("Connecting"));
    wxStaticBoxSizer *wxCounter         = new wxStaticBoxSizer(wxVERTICAL, framework_panel, wxT("Counter"));
    wxStaticBoxSizer *wxMonochromator   = new wxStaticBoxSizer(wxVERTICAL, framework_panel, wxT("Monochromator"));
    wxStaticBoxSizer *wxAdjustment      = new wxStaticBoxSizer(wxVERTICAL, framework_panel, wxT("Adjustment"));



    control_sizer -> Add(wxCounter,         0, wxEXPAND, 0); // wxEXPAND      wxSHAPED | wxALL, 2
    control_sizer -> Add(new wxStaticText(framework_panel, -1, wxT("")), 0, wxEXPAND);
    control_sizer -> Add(wxMonochromator,   0, wxEXPAND, 0);
    monoBackground = new wxCheckBox(framework_panel, -1, wxT( "Subtract background" )); // https://docs.wxwidgets.org/3.0/classwx_check_box.html
    control_sizer -> Add(monoBackground, 0, wxCENTER);
    monoStart = new wxButton(framework_panel, idMonoStart, wxT("Start measurement"));
    control_sizer -> Add(monoStart, 0, wxCENTER); //  | wxEXPAND
    monoStop = new wxButton(framework_panel, idMonoStop, wxT("Stop"));
    control_sizer -> Add(monoStop, 0, wxCENTER); //  | wxEXPAND
    control_sizer -> Add(new wxStaticText(framework_panel, -1, wxT("")), 0, wxEXPAND);
    control_sizer -> Add(wxAdjustment,      0, wxEXPAND, 0);
    adjustmentBackground = new wxCheckBox(framework_panel, -1, wxT( "Subtract background" ));
    control_sizer -> Add(adjustmentBackground, 0, wxCENTER);
    adjustmentStart = new wxButton(framework_panel, idAdjustmentStart, wxT("Start adjustment"));
    control_sizer -> Add(adjustmentStart, 0, wxCENTER); //  | wxEXPAND
    adjustmentStop = new wxButton(framework_panel, idAdjustmentStop, wxT("Stop"));
    control_sizer -> Add(adjustmentStop, 0, wxCENTER); //  | wxEXPAND

    monoStart       -> Disable();
    adjustmentStart -> Disable();
    monoStop        -> Disable();
    adjustmentStop  -> Disable();

// counter =========================
    wxGridSizer *wxCounterGrid  = new wxGridSizer(5,2,3,3);

    counterComChoice = new wxChoice(framework_panel, idCounterCOM, wxDefaultPosition, wxDefaultSize, 9, Ports, wxCB_SORT);
    wxCounterGrid -> Add(counterComChoice, 0, wxSHAPED);
    counterComConnect = new wxButton(framework_panel, idCounterConnect, wxT("Connect"));
    wxCounterGrid -> Add(counterComConnect, 0, wxSHAPED);
    wxCounterGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Trigger")), 0, wxSHAPED);
    counterSlope = new wxChoice(framework_panel, idCounterSlope, wxDefaultPosition, wxDefaultSize, 2, slopes); //, wxCB_SORT
    wxCounterGrid -> Add(counterSlope, 0, wxSHAPED);
    wxCounterGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Discriminator, mV")), 0, wxSHAPED);
    counterDiscr = new wxSpinCtrl(framework_panel, idCounterDiscr, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, -2000, 2000, 300, wxT("smth"));
    wxCounterGrid -> Add(counterDiscr, 0, wxSHAPED);
    wxCounterGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Sample time, ms")), 0, wxSHAPED);
    counterSampleTime = new wxSpinCtrl(framework_panel, idCounterSampleTime, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 0, 50000, 200, wxT("smth"));
    wxCounterGrid -> Add(counterSampleTime, 0, wxSHAPED);
    wxCounterGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Dead time, ms")), 0, wxSHAPED);
    counterDeadTime = new wxSpinCtrl(framework_panel, idCounterDeadTime, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 0, 5000, 100, wxT("smth"));
    wxCounterGrid -> Add(counterDeadTime, 0, wxSHAPED);


    wxCounter -> Add(wxCounterGrid, 0, wxSHAPED); // wxSHAPED

    counterComChoice -> SetSelection(counter.GetPortNumber() - 1); // default - 3, real - COM4
    counterSlope -> SetSelection(counter.GetFrontSlope());
    counterDiscr -> SetValue(counter.GetDiscLevel());
    counterSampleTime -> SetValue(counter.GetSampleTime());
    counterDeadTime -> SetValue(counter.GetDeadTime());


// monochromator =============================


//wxArrayString* str = new wxArrayString(4, allow);



    wxGridSizer *wxMonochromatorGrid = new wxGridSizer(6,2,3,3); //wxSHAPED);

    monoComChoice = new wxChoice(framework_panel, idMonoCOM, wxDefaultPosition, wxDefaultSize, 9, Ports, wxCB_SORT); // wxID_ANY
    wxMonochromatorGrid -> Add(monoComChoice, 0, wxSHAPED); //new wxChoice(framework_panel, idMonoCOM, wxDefaultPosition, wxDefaultSize, 8, Ports, wxCB_SORT), 0, wxSHAPED);
    monoComConnect = new wxButton(framework_panel, idMonoConnect, wxT("Connect"));
    wxMonochromatorGrid -> Add(monoComConnect, 0, wxSHAPED);
    wxMonochromatorGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Grating")), 0, wxSHAPED);
    monoGrate = new wxChoice(framework_panel, idMonoGrate, wxDefaultPosition, wxDefaultSize, 2, grates, 0); //wxCB_SORT
    wxMonochromatorGrid -> Add(monoGrate, 0, wxSHAPED);

    wxMonochromatorGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Start, nm")), 0, wxSHAPED);
    monoWavelengthStart = new wxSpinCtrl(framework_panel, idMonoWavelengthStart, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 0, 5000, 400, wxT("smth")); //
    wxMonochromatorGrid -> Add(monoWavelengthStart, 0, wxSHAPED);
    wxMonochromatorGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Stop, nm")), 0, wxSHAPED);
    monoWavelengthStop = new wxSpinCtrl(framework_panel, idMonoWavelengthStop, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 0, 5000, 700, wxT("smth")); //  | wxTE_PROCESS_ENTER
    wxMonochromatorGrid -> Add(monoWavelengthStop, 0, wxSHAPED);
/**/
    wxMonochromatorGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Step, nm")), 0, wxSHAPED);
    monoWavelengthStep = new wxSpinCtrl(framework_panel, idMonoWavelengthStep, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 1, 300, 10, wxT("smth"));
    wxMonochromatorGrid -> Add(monoWavelengthStep, 0, wxSHAPED);
    wxMonochromatorGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Cycles")), 0, wxSHAPED);
    monoCycles = new wxSpinCtrl(framework_panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 1, 256, 1, wxT("smth"));
    wxMonochromatorGrid -> Add(monoCycles,  0, wxSHAPED);
//    wxMonochromatorGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Background")), 0, wxSHAPED);
//    wxMonochromatorGrid -> Add(new wxButton(framework_panel, -1, wxT("Start4")), 0, wxSHAPED);


    wxMonochromator -> Add(wxMonochromatorGrid, 0, wxSHAPED);

    monoComChoice -> SetSelection(monochromator.GetPortNumber() - 1); // default - 3, real - COM4
//    monoComChoice->SetSelection(0); // 3 - COM 4, 0 - COM1, логично, это номер позиции в списке, начало с нуля.

    monoGrate -> SetSelection(monochromator.GetGrate() - 1);
    monoWavelengthStart -> SetValue((int)monochromator.GetWavelengthStart());
    monoWavelengthStop  -> SetValue((int)monochromator.GetWavelengthStop());
    monoWavelengthStep  -> SetValue(monochromator.GetWavelengthStep());






// adjustment =============================
    wxGridSizer *wxAdjustmentGrid = new wxGridSizer(1,2,3,3);

    wxAdjustmentGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Wavelength, nm")), 0, wxSHAPED);
    adjustmentWavelength = new wxSpinCtrl(framework_panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 0, 5000, 500, wxT("smth"));
    wxAdjustmentGrid -> Add(adjustmentWavelength, 0, wxSHAPED);


    wxAdjustment -> Add(wxAdjustmentGrid, 0, wxSHAPED);




// end of control panel


    framework_sizer->Add(control_sizer, 0, wxSHAPED); // wxEXPAND



    // https://forums.wxwidgets.org/viewtopic.php?t=27372
    // https://forums.wxwidgets.org/viewtopic.php?t=39273
    // https://sourceforge.net/projects/wxmathplot/ 2021
    // https://wxmathplot.sourceforge.io/docs/ 2007

    // http://wxcode.sourceforge.net/components/freechart/
    // http://plplot.sourceforge.net/


//    wxButton *ok2 = new wxButton(framework_panel, -1, wxT("Ok2"));
  //  main_sizer->Add(ok2);


//    wxSize  graph_size = wxSize(100,300);

    framework_graph = new mpWindow(framework_panel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);

//    framework_graph.AddLayer(mpInfoCoords());
//    framework_graph.AddLayer(mpScaleX(wxT("Counts"), mpALIGN_BOTTOM, true));
//   framework_graph.AddLayer(mpScaleY(wxT("Wavelength, nm"), mpALIGN_LEFT, true));

/*
    static mpWindow *framework_graph =  new mpWindow(framework_panel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);

  // сделать свой класс, наследник mpFXYVector, и далее его подключить. а оси-координаты можно и тут оставить... хотя перерисовку-то надо вызвать
    static mpInfoCoords *frame_coord = new mpInfoCoords();
    framework_graph->AddLayer(frame_coord);
    // mpWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long flags = 0);

    static mpFXYVector *frameworkVector = new mpFXYVector();
    frameworkVector->SetData(vWavelength, vAveragedCounts);
    frameworkVector->SetContinuity(true);
    frameworkVector->SetPen(wxPen(wxColor(0xFF, 0x00, 0x00), 2, wxSOLID)); // wxPen wxRED_PEN   use wxPENSTYLE_XXX wxPENSTYLE_RED

    framework_graph->AddLayer(frameworkVector);


    static mpScaleY *scaleY = new mpScaleY(wxT("Counts"), mpALIGN_BOTTOM, true);
    static mpScaleX *scaleX = new mpScaleX(wxT("Wavelength, nm"), mpALIGN_LEFT, true);

    framework_graph->AddLayer(scaleX);
    framework_graph->AddLayer(scaleY);
    if (monoWavelengthStart->GetValue() > monoWavelengthStop->GetValue())
    framework_graph->Fit(monoWavelengthStop->GetValue(), monoWavelengthStart->GetValue(), 0, 100);
    else
    framework_graph->Fit(monoWavelengthStart->GetValue(), monoWavelengthStop->GetValue(), 0, 100);



*/

    framework_sizer->Add(framework_graph, 1, wxEXPAND);







/*    class WXDLLIMPEXP_MATHPLOT mpScaleY;
    void mpScaleY::Plot(wxDC &dc, mpWindow &w)

    // https://cpp.hotexamples.com/examples/-/NumberAxis/-/cpp-numberaxis-class-examples.html
    // https://forums.wxwidgets.org/viewtopic.php?f=27&t=46942

*/






    framework_panel->SetSizer(framework_sizer);

    Centre();
}


Framework_wx_pureFrame::~Framework_wx_pureFrame()
{
    // тут-то надо окна удалять или нет? или только всяякие массивы в куче?
    // https://forums.wxwidgets.org/viewtopic.php?t=46545

    // delete ...mpWindow...   wxSpinCtrl ... wxStaticText etc.


    if(monochromator.CheckState())
    {
        monochromator.Close();
    }
    if(counter.CheckState())
    {
        counter.Close();
    }


}

void Framework_wx_pureFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void Framework_wx_pureFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void Framework_wx_pureFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void Framework_wx_pureFrame::OnSaveAs(wxCommandEvent &event)
{
	wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save File As..."), wxEmptyString, wxEmptyString, _("Text files (*.txt)|*.txt|ASCII Files (*.asc)|*.asc"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
	{
		SetTitle(wxString("Data saved - ") << SaveDialog->GetFilename());

        wxString path;
		path.append( SaveDialog->GetDirectory() );
		path.append( wxFileName::GetPathSeparator() );
		path.append( SaveDialog->GetFilename() );

		std::ofstream fsave;
		fsave.open(path, std::ios::out); (path); //  | ios::binary

        if (fsave.is_open()) // если файл не открыт
        {
            // fsave << "Wavelength (nm) \t Counts" << std::endl;
            for(int i = 0; i < vAveragedCounts.size(); i++) //
            {
                fsave << vWavelength[i] << "\t" << vAveragedCounts[i] << std::endl;
            }

            fsave.close();
        }
        else
        {
           wxMessageBox( wxT("Can not open file"), wxT("Save file"), wxOK | wxICON_INFORMATION);

        }



	}

	// Clean up after ourselves
	SaveDialog->Destroy();

 }


// counter ============================================
void Framework_wx_pureFrame::CounterConnect(wxCommandEvent &event)
{
    if(counter.CheckState())
    {   // already opened
        counter.Close();
        counterComConnect->SetLabel("Connect");
        counterComChoice -> Enable();
    }
    else
    { // is not opened yet
        if(counter.Open(counter.GetPortNumber()))
        {
            // change button label to Disconnect
            counterComConnect->SetLabel("Disconnect");
            counterComChoice -> Disable();
        }
        else
        {
            counter.Close();
            // nothing to do or  make an error message - currently we already have a lot of MessageBoxes from Open
        }

    }

}

void Framework_wx_pureFrame::CounterCOM(wxCommandEvent &event)
{
    counter.SetPortNumber((counterComChoice -> GetSelection()) + 1);
}

void Framework_wx_pureFrame::CounterSlope(wxCommandEvent &event)
{

   /* if(counterSlope -> GetSelection()) // не помогло :)
        counter.SetFrontSlope(FALL);
    else
        counter.SetFrontSlope(RISE);
        */
   counter.SetFrontSlope(static_cast<Slope>(counterSlope -> GetSelection()));
}

void Framework_wx_pureFrame::CounterDiscr(wxCommandEvent &event)
{
    if(!counter.SetDiscLevel(counterDiscr -> GetValue()))
        counterDiscr -> SetValue(counter.GetDiscLevel());
}

void Framework_wx_pureFrame::CounterSampleTime(wxCommandEvent &event)
{
    if(!counter.SetSampleTime(counterSampleTime -> GetValue()))
        counterSampleTime -> SetValue(counter.GetSampleTime());
}

void Framework_wx_pureFrame::CounterDeadTime(wxCommandEvent &event)
{
    if(!counter.SetDeadTime(counterDeadTime -> GetValue()))
        counterDeadTime -> SetValue(counter.GetDeadTime());
}








// monochromator =====================================

void Framework_wx_pureFrame::MonoConnect(wxCommandEvent &event)
{
    if(monochromator.CheckState())
    {   // already opened
        monochromator.Close();
        monoComConnect->SetLabel("Connect");
        monoStart -> Disable();
        monoComChoice -> Enable();
        adjustmentStart -> Disable();

    }
    else
    { // is not opened yet
        if(monochromator.Open(monochromator.GetPortNumber()))
        {
            // change button label to Disconnect
            monoComConnect->SetLabel("Disconnect");
            monoStart -> Enable();
            monoComChoice -> Disable();
            adjustmentStart -> Enable();

            // вызвать функцию получения списка решеток из монохроматора? чтобы загрузить его в список решеток, например.
            // но эт в будущем, а пока ставить что есть.

        }
        else
        {
            monochromator.Close();
            // nothing to do or  make an error message - currently we already have a lot of MessageBoxes from Open
        }


    }

}

void Framework_wx_pureFrame::MonoCOM(wxCommandEvent &event)
{
    monochromator.SetPortNumber((monoComChoice->GetSelection()) + 1);
}

void Framework_wx_pureFrame::MonoGrate(wxCommandEvent &event)
{
        monochromator.SetGrate(monoGrate->GetSelection() + 1);


}

void Framework_wx_pureFrame::MonoWavelengthStart(wxCommandEvent &event)
{
    monochromator.SetWavelengthStart(monoWavelengthStart->GetValue());

/*
    if(!counter.SetSampleTime(counterSampleTime -> GetValue()))
        counterSampleTime -> SetValue(counter.GetSampleTime());
        */

}

void Framework_wx_pureFrame::MonoWavelengthStop(wxCommandEvent &event)
{
    monochromator.SetWavelengthStop(monoWavelengthStop->GetValue());
}

void Framework_wx_pureFrame::MonoWavelengthStep(wxCommandEvent &event)
{
    monochromator.SetWavelengthStep(monoWavelengthStep->GetValue());
}

void Framework_wx_pureFrame::MonoStart(wxCommandEvent &event)
{
    if(!counter.CheckState())
    {
        wxMessageDialog *dial = new wxMessageDialog (NULL, wxT( "Counter is not connected" ) , wxT( " Error" ) , wxOK | wxICON_ERROR);
        dial -> ShowModal();
        return;
    }

// первый отсчет происходит еще во время перестроения!  но никак не задержать, пробовал и присвоение значения, и while
// причем это чревато сдвигом спектра и при обычной записи, так что надо это исправить и внедрить в основной цикл.
// добавление проверки возвращаемого значения ничего не дает, все также преждевременный старт.
// if(!x) does not work too
// volatile x does not help


/*
    volatile int x = 0;
//    double wavelength = monochromator.GetWavelengthStart();
    //std::thread t1{&ARC_MONOCHROMATOR::monochromator.GoToWavelength, wavelength};
//    t1.join();

            auto tp = std::async([&monochromator, wavelength]{return monochromator.GoToWavelength(wavelength);});
            x = tp.get();


*/


    framework_graph->DelAllLayers(true, true);

    mpInfoCoords *frame_coord = new mpInfoCoords();
    framework_graph->AddLayer(frame_coord);

    mpFXYVector *frameworkVector = new mpFXYVector();
//    frameworkVector->SetData(vWavelength, vAveragedCounts);
    frameworkVector->SetContinuity(true);
    frameworkVector->SetPen(wxPen(wxColor(0xFF, 0x00, 0x00), 2, wxSOLID));

    framework_graph->AddLayer(frameworkVector);

    mpScaleY *scaleY = new mpScaleY(wxT("Counts"), mpALIGN_BOTTOM, true);
    mpScaleX *scaleX = new mpScaleX(wxT("Wavelength, nm"), mpALIGN_LEFT, true);

    framework_graph->AddLayer(scaleX);
    framework_graph->AddLayer(scaleY);

    // а если делать фит после заполнения вектора длин волн - то и думать сравнивать не надо...
//    if (monoWavelengthStart->GetValue() > monoWavelengthStop->GetValue())
//    framework_graph->Fit(monoWavelengthStop->GetValue(), monoWavelengthStart->GetValue(), 0, 100);
//    else
//    framework_graph->Fit(monoWavelengthStart->GetValue(), monoWavelengthStop->GetValue(), 0, 100);



    // теперь проверить требуется ли вычитать фон -
    // и если надо, то вызвать функцию, возвращающую этот фон, тк вызывать еще придется из окна юстировки

    monoStart -> Disable();
    monoStop -> Enable();
    adjustmentStart -> Disable();

    measurementStop = FALSE;
    SetTitle(wxString("Framework_wx"));


// a bit easier way - idle and timer https://wiki.wxwidgets.org/Making_a_render_loop
// Alternatives to Multithreading   https://flylib.com/books/en/3.138.1.147/1/
//  wxYield(); /
// wxSafeYield(); - disables the user input to all program windows before calling wxYield and re-enables it again afterwards.





    // и если надо, то создать функцию, возвращающую фон, тк вызывать еще придется из окна юстировки
    // например int GetBackground(int average)
    int background = 0;
    if(monoBackground ->GetValue())
    {
        wxMessageBox( wxT("Close the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);

        int backgroundAveraging = 4;
        for(int i = 0; i < backgroundAveraging; i++)
        {

            counter.Reset();
            counter.Start();
            usleep(1000*counter.GetSampleTime());

            background += counter.GetCount();
        }
        background =  background / backgroundAveraging;

        wxMessageBox( wxT("Open the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);
    }

    int scanDirection = 1;
    if(monochromator.GetWavelengthStart() > monochromator.GetWavelengthStop())
    {
        scanDirection = -1;
    }

    int elements = 1 + (int) scanDirection * (monochromator.GetWavelengthStop() - monochromator.GetWavelengthStart()) / (double)monochromator.GetWavelengthStep();






// with push    =====================================

    std::vector <int> vAccumulatedCounts(elements, 0);
    vWavelength.resize(0);
    vAveragedCounts.resize(0);


    for(int cycle = 0; cycle < (monoCycles -> GetValue()); cycle++)
    {

        for(int element = 0; element < elements; element++)
        {


            monochromator.GoToWavelength(monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep());

            wxYield();
            if(measurementStop)
            break;

            counter.Reset();
            counter.Start();
            usleep(1000*counter.GetSampleTime());



            if(cycle == 0)
            {
                vWavelength.push_back(monochromator.GetWavelengthStart() + element * scanDirection * monochromator.GetWavelengthStep());
                vAveragedCounts.push_back((signed int)counter.GetCount() - background);
                vAccumulatedCounts[element] = vAveragedCounts[element];

            }
            else
            {
                vAccumulatedCounts[element] = vAccumulatedCounts[element] + counter.GetCount();

                vAveragedCounts[element] = (int)((signed int)vAccumulatedCounts[element] / (cycle + 1)) - background;

            }

            frameworkVector -> SetData(vWavelength, vAveragedCounts);

            auto vMinmax = std::minmax_element(vAveragedCounts.begin(), vAveragedCounts.end(), compLess);

            framework_graph->Fit(monoWavelengthStart->GetValue(), monoWavelengthStop->GetValue(), *vMinmax.first, *vMinmax.second); //0, 100);



  /*

























// without push ==========================
    std::vector <int> vAccumulatedCounts(elements, 0);

    vWavelength.resize(elements);
    for(int i = 0; i < elements; i++)
    {
        vWavelength[i] = monochromator.GetWavelengthStart() + i * scanDirection * monochromator.GetWavelengthStep();
    }

    vAveragedCounts.resize(elements);
    std::fill(vAveragedCounts.begin(), vAveragedCounts.end(), 0);



    for(int cycle = 0; cycle < (monoCycles -> GetValue()); cycle++)
    {

        for(int element = 0; element < elements; element++)
        {

//wxMessageBox( wxT("Next step"), wxT("Measurement"), wxOK | wxICON_INFORMATION);

        // запускается, возвращает 1 но не ждет... даже с проверкой перед циклом.
 //           double wavelength = monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep();
   //         auto tp = std::async([&monochromator, wavelength]{return monochromator.GoToWavelength(wavelength);});
     //       int x = tp.get();

            monochromator.GoToWavelength(monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep());

//            int x = monochromator.GoToWavelength(monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep());
//            if((cycle == 0) && (element == 0))
 //           usleep(3000000);

            wxYield();
            if(measurementStop)
            break;

            counter.Reset();
            counter.Start();
            usleep(1000*counter.GetSampleTime());

            vAccumulatedCounts[element] = vAccumulatedCounts[element] + counter.GetCount();

            vAveragedCounts[element] = (int)(vAccumulatedCounts[element] / (cycle + 1)) - background;





           frameworkVector -> SetData(vWavelength, vAveragedCounts);
//wxMessageBox( wxT("Next step"), wxT("Measurement"), wxOK | wxICON_INFORMATION);

// from FXYV: This method DOES NOT refresh the mpWindow. UpdateAll() or just Fit() if we want to adjust plot
// крашится любое обращение к frameworkVector или framework_graph, хотя их объявление висит в заголовочном. Может надо тут создавать свое? Сработает один раз? :)




//        frameworkVector->SetContinuity(true);

        //frameworkVector -> Clear();
  //         framework_graph->Fit();
            // This method DOES NOT refresh the mpWindow; do it manually.
         //   framework_graph -> UpdateAll(); // ??
        //  framework_graph -> Refresh();
         //     framework_graph->Fit(monoWavelengthStart->GetValue(), monoWavelengthStop->GetValue(), 0, 100);
         // if(scanDirection == 1)
         framework_graph->Fit(); //  может масштабирование делать если разность мин-макс превышает 100?














   */





        }
        if(measurementStop)
        break;

    }


    monoStart -> Enable();
    monoStop -> Disable();
    adjustmentStart -> Enable();
    measurementStop = FALSE;

    SetTitle(wxString("Framework_wx   Data not saved"));





}

void Framework_wx_pureFrame::MonoStop(wxCommandEvent &event)
{
    monoStop -> Disable();

    measurementStop = TRUE;
}


void Framework_wx_pureFrame::AdjustmentStart(wxCommandEvent &event)
{

    monochromator.GoToWavelength(adjustmentWavelength -> GetValue());

    framework_graph->DelAllLayers(true, true);

    mpInfoCoords *frame_coord = new mpInfoCoords();
    framework_graph->AddLayer(frame_coord);

    mpFXYVector *frameworkVector = new mpFXYVector();
//    frameworkVector->SetData(vWavelength, vAveragedCounts);
    frameworkVector->SetContinuity(true);
    frameworkVector->SetPen(wxPen(wxColor(0xFF, 0x00, 0x00), 2, wxSOLID));

    framework_graph->AddLayer(frameworkVector);

    mpScaleY *scaleY = new mpScaleY(wxT("Counts"), mpALIGN_BOTTOM, true);
    mpScaleX *scaleX = new mpScaleX(wxT("Time, sec"), mpALIGN_LEFT, true);

    framework_graph->AddLayer(scaleX);
    framework_graph->AddLayer(scaleY);



    monoStart -> Disable();
    measurementStop = FALSE;
    adjustmentStop -> Enable();
    adjustmentStart -> Disable();
    adjustmentWavelength -> Disable();



    int background = 0;
    if(adjustmentBackground ->GetValue())
    {
        wxMessageBox( wxT("Close the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);

        int backgroundAveraging = 4;
        for(int i = 0; i < backgroundAveraging; i++)
        {

            counter.Reset();
            counter.Start();
            usleep(1000*counter.GetSampleTime());

            background += counter.GetCount();
        }
        background =  background / backgroundAveraging;

        wxMessageBox( wxT("Open the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);
    }


    int elements = 300;

    std::deque <double> qAdjCounts; //(elements, 0);
    std::deque <double> qAdjTime;//(elements, 0);

//    my_queue<double> qAdjCounts;
//    my_queue<double> qAdjTime;

    std::vector <double> vAdjCounts; //(elements); //(elements, 0);
    std::vector <double> vAdjTime; //(elements);//(elements, 0);


/*    for(int i = 0; i < elements; i++)
    {
        qAdjCounts.push_front(0);
        qAdjTime.push_front((-elements + i)*(double)counter.GetSampleTime()/1000);

    }
*/
    frameworkVector -> SetData(vAdjTime, vAdjCounts);



    // есть копирование по элеметам, от первого до последнего...
    // или все-таки самостоятельно сделать перенос элементов, без копирования. хотя м.б. СТД быстрее делает


    double adjTime = 0;
    int vCount = 0;
    do
    {

        adjTime = vCount*(double)counter.GetSampleTime()/1000;
       // count
        counter.Reset();
        counter.Start();
        usleep(1000*counter.GetSampleTime());

        // insert first element
        // push
        qAdjCounts.push_front((signed int)counter.GetCount() - background);
        qAdjTime.push_front(adjTime);

//        adjTime += (double)counter.GetSampleTime()/1000;

        // remove last element
        // pop

        if(vCount < elements)
        {
            vAdjCounts.resize(vCount + 1);
            vAdjTime.resize(vCount + 1);
        }

        if(vCount >= elements)
        {
            qAdjCounts.pop_back();
            qAdjTime.pop_back();

        }
        wxYield();

        std::copy(qAdjCounts.begin(), qAdjCounts.end(), vAdjCounts.begin());
        std::copy(qAdjTime.begin(), qAdjTime.end(), vAdjTime.begin());

//        std::vector <double>& vAdjCounts  = qAdjCounts.to_vector();
//        std::vector <double>& vAdjTime = qAdjTime.to_vector();
 //       auto vAdjCounts  = AcceptVectors({qAdjCounts.begin(), qAdjCounts.end()});
 //       auto vAdjTime = AcceptVectors({qAdjTime.begin(), qAdjTime.end()}); // std::vector <double>


        frameworkVector -> SetData(vAdjTime, vAdjCounts);



     //   framework_graph->Fit();


        auto vMinmax = std::minmax_element(vAdjCounts.begin(), vAdjCounts.end(), compLess);

        double minElement = *vMinmax.first;
        double maxElement = *vMinmax.second;

        framework_graph->Fit(adjTime - (double)counter.GetSampleTime()*elements/1000, adjTime, *vMinmax.first, *vMinmax.second); //0, 100);

   //     std::pair<double> vMinMax = std::minmax_element(vAdjCounts.begin(), vAdjCounts.end());


 //       double minElement = *std::min_element(vAdjCounts.begin(), vAdjCounts.end());
  //      double maxElement = *std::max_element(vAdjCounts.begin(), vAdjCounts.end());

     //   const auto [min, max] = std::minmax_element(vAdjCounts.begin(), vAdjCounts.end());
 //       framework_graph->Fit(adjTime - (double)counter.GetSampleTime()*elements/1000, adjTime, *min, *max); //0, 100); ////vMinMax.first, vMinMax.second); //

     //   framework_graph->Fit(adjTime - (double)counter.GetSampleTime()*elements/1000, adjTime, 0, 1000); //minElement, maxElement);

     vCount++;

    }
    while(!measurementStop);








    monoStart -> Enable();
    adjustmentWavelength -> Enable();
    adjustmentStop -> Disable();
    adjustmentStart -> Enable();

}

void Framework_wx_pureFrame::AdjustmentStop(wxCommandEvent &event)
{
    measurementStop = TRUE;
}
