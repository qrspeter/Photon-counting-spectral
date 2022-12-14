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

#include <unistd.h> // usleep(TIME_TO_SLEEP); ms
// sleep_for() определяется стандартом C++11, а Sleep() определяется стандартом Windows API
// https://coderoad.ru/53072750/
// https://forums.wxwidgets.org/viewtopic.php?t=36579 usleep is no worse than wxStopWatch
// wxStopWatch is the *only* way under MSW for accurately measuring small time intervals, because it uses the PerformanceCounter API. All other methods work with a resolution of ~15ms



#include <vector>

#include <chrono>
#include <future>
#include <thread> // https://habr.com/ru/post/182610/

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

std::vector <int> vWavelength;
std::vector <int> vAveragedCounts;


// Create a std::promise object
std::promise<void> exitSignal;
//Fetch std::future object associated with promise
std::future<void> futureObj = exitSignal.get_future();
std::future_status status = std::future_status::ready;


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
    EVT_MENU(idMenuSave, Framework_wx_pureFrame::OnSave)
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
//    EVT_TEXT(idMonoWavelengthStop, Framework_wx_pureFrame::MonoWavelengthStop)
    EVT_TEXT_ENTER(idMonoWavelengthStop, Framework_wx_pureFrame::MonoWavelengthStop)
//    EVT_SPINCTRL(idMonoWavelengthStop, Framework_wx_pureFrame::MonoWavelengthStop) // invalid static_cast from....
//    EVT_TEXT(idMonoWavelengthStep, Framework_wx_pureFrame::MonoWavelengthStep)
    EVT_TEXT_ENTER(idMonoWavelengthStep, Framework_wx_pureFrame::MonoWavelengthStep)
    // EVT_LISTBOX_DCLICK
    EVT_BUTTON(idMonoStart, Framework_wx_pureFrame::MonoStart)


    EVT_BUTTON(idAdjustmentStart, Framework_wx_pureFrame::AdjustmentStart)
END_EVENT_TABLE()

Framework_wx_pureFrame::Framework_wx_pureFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
// Peter. Menu described there https://docs.wxwidgets.org/3.0/overview_helloworld.html
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuSave, _("&Save\tCtrl-s"), _("Save data"));
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
    control_sizer -> Add(new wxStaticText(framework_panel, -1, wxT("")), 0, wxEXPAND);
    control_sizer -> Add(wxAdjustment,      0, wxEXPAND, 0);
    adjustmentBackground = new wxCheckBox(framework_panel, -1, wxT( "Subtract background" ));
    control_sizer -> Add(adjustmentBackground, 0, wxCENTER);
    adjustmentStart = new wxButton(framework_panel, idAdjustmentStart, wxT("Start adjustment"));
    control_sizer -> Add(adjustmentStart, 0, wxCENTER); //  | wxEXPAND

    monoStart       -> Disable();
    adjustmentStart -> Disable();

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
    wxCounterGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Integrate time, ms")), 0, wxSHAPED);
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
    monoWavelengthStop = new wxSpinCtrl(framework_panel, idMonoWavelengthStop, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 5000, 700, wxT("smth")); //  | wxTE_PROCESS_ENTER
    wxMonochromatorGrid -> Add(monoWavelengthStop, 0, wxSHAPED);
/**/
    wxMonochromatorGrid -> Add(new wxStaticText(framework_panel, -1, wxT("Step, nm")), 0, wxSHAPED);
    monoWavelengthStep = new wxSpinCtrl(framework_panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 1, 300, 10, wxT("smth"));
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
    wxAdjustmentGrid -> Add(new wxSpinCtrl(framework_panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 300, 5000, 1, wxT("smth")), 0, wxSHAPED);


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

    mpWindow *framework_graph =  new mpWindow(framework_panel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    // mpWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long flags = 0);

    mpScaleY *scaleY = new mpScaleY(wxT("Counts"), mpALIGN_BOTTOM, true);
    mpScaleX *scaleX = new mpScaleX(wxT("Wavelength, nm"), mpALIGN_LEFT, true);

    framework_graph->AddLayer(scaleX);
    framework_graph->AddLayer(scaleY);
    framework_graph->Fit(400, 900, 0, 100);


/*    class WXDLLIMPEXP_MATHPLOT mpScaleY;
    void mpScaleY::Plot(wxDC &dc, mpWindow &w)

    // https://cpp.hotexamples.com/examples/-/NumberAxis/-/cpp-numberaxis-class-examples.html
    // https://forums.wxwidgets.org/viewtopic.php?f=27&t=46942

*/









    framework_sizer->Add(framework_graph, 1, wxEXPAND);




    framework_panel->SetSizer(framework_sizer);

    Centre();
}


Framework_wx_pureFrame::~Framework_wx_pureFrame()
{
    // тут-то надо окна удалять или нет? или только всяякие массивы в куче?
    // https://forums.wxwidgets.org/viewtopic.php?t=46545

    if(monochromator.CheckState())
    {
        monochromator.Close();
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

void Framework_wx_pureFrame::OnSave(wxCommandEvent &event)
{

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

    }
    else
    { // is not opened yet
        if(monochromator.Open(monochromator.GetPortNumber()))
        {
            // change button label to Disconnect
            monoComConnect->SetLabel("Disconnect");
            monoStart -> Enable();
            monoComChoice -> Disable();

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
/*
    // тут проверять запущен ли процесс, и если да то то завершать его и переименовывать кнопку обратно.
    // Иначе - начинать процесс.
    // также функция в процессе  по завершению должен переименовать кнопку обратно.

    // https://en.cppreference.com/w/cpp/thread/future/wait_for
    // https://en.cppreference.com/w/cpp/thread/future/valid

    // atomic, async ....
    // https://stackoverflow.com/questions/9094422/how-to-check-if-a-stdthread-is-still-running
    // Если нужно узнать запущен ли поток то проверяйте на joinable.

    // auto myFuture = std::async(std::launch::async, myFunction); // https://www.acodersjourney.com/top-20-cplusplus-multithreading-mistakes/

//    futureObj.valid() // Checks if the future refers to a shared state.

// http://cool-emerald.blogspot.com/2020/01/multithreading-with-wxwidgets.html

    status = futureObj.wait_for(std::chrono::milliseconds(0));
    //declare globally:

    if(status == std::future_status::ready) // если поток существует и не закончен - остановить, отсутствует - запустить.
    {
        monoStart -> SetLabel("Stop");
        std::thread th(&threadMeasurement, std::move(futureObj));
    }
    else
    {
        exitSignal.set_value();
//        th.join();
    }


*/

// a bit easier way - idle and timer https://wiki.wxwidgets.org/Making_a_render_loop
// Alternatives to Multithreading   https://flylib.com/books/en/3.138.1.147/1/
//  wxYield(); /
// wxSafeYield(); - disables the user input to all program windows before calling wxYield and re-enables it again afterwards.


    monochromator.GoToWavelength(monochromator.GetWavelengthStart());

    // и если надо, то создать функцию, возвращающую фон, тк вызывать еще придется из окна юстировки
    // например int GetBackground(int average)
    int background = 0;
    if(monoBackground ->GetValue())
    {
        wxMessageBox( wxT("Close the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);
//        MessageBox(NULL, "Close the laser", "Background", MB_OK);

        int backgroundAveraging = 4;
        for(int i = 0; i < backgroundAveraging; i++)
        {

            counter.Start();
            usleep(counter.GetSampleTime());

            background =+ counter.GetCount();
        }
        background =  background / backgroundAveraging;

        wxMessageBox( wxT("Open the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);
//        MessageBox(NULL, "Open the laser", "Background", MB_OK);
    }

    int scanDirection = 1;
    if(monochromator.GetWavelengthStart() < monochromator.GetWavelengthStop())
        scanDirection = -1;

    int elements = (int) scanDirection * (monochromator.GetWavelengthStart() - monochromator.GetWavelengthStop()) / (double)monochromator.GetWavelengthStep();
    std::vector <int> vAccumulatedCounts(elements, 0);
    vWavelength.resize(elements);
    std::fill(vWavelength.begin(), vWavelength.end(), 0);
    vAveragedCounts.resize(elements);
    std::fill(vAveragedCounts.begin(), vAveragedCounts.end(), 0);

    for(int cycle = 0; cycle < monoCycles -> GetValue(); cycle++) // (тогда можно число циклов на ходу поменять)
    {

        for(int element = 0; element < elements; element++)
        {

    // каждый цикл проверять состояние кнопки "старт", которая тут уже переменована в "стоп", или перезватыать событие от этой кнопки?
    // monoStart -> SetLabel("Start");
    // можно проверить была она нажата или нет?



            monochromator.GoToWavelength(monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep());

            counter.Start();
            usleep(counter.GetSampleTime());
            if(element == 0)
            vWavelength[element] = monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep();
            vAccumulatedCounts[element] =+ counter.GetCount();

            vAveragedCounts[element] = (int)(vAccumulatedCounts[element] / (cycle + 1)) - background;


        }
    }
    monoStart -> SetLabel("Start");


}

// wx. https://docs.wxwidgets.org/trunk/classwx_thread.html
// MyThread *m_pThread; m_pThread = new MyThread(this); and... if (m_pThread)  // does the thread still exist?

// start-stop button with wx and thread https://forums.wxwidgets.org/viewtopic.php?t=39869

// https://thispointer.com/c11-how-to-stop-or-terminate-a-thread/
// https://stackoverflow.com/questions/9094422/how-to-check-if-a-stdthread-is-still-running

void Framework_wx_pureFrame::threadMeasurement(std::future<void> futureObj)
{

    monochromator.GoToWavelength(monochromator.GetWavelengthStart());

    // и если надо, то создать функцию, возвращающую фон, тк вызывать еще придется из окна юстировки
    // например int GetBackground(int average)
    int background = 0;
    if(monoBackground ->GetValue())
    {
        wxMessageBox( wxT("Close the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);
//        MessageBox(NULL, "Close the laser", "Background", MB_OK);

        int backgroundAveraging = 4;
        for(int i = 0; i < backgroundAveraging; i++)
        {

            counter.Start();
            usleep(counter.GetSampleTime());

            background =+ counter.GetCount();
        }
        background =  background / backgroundAveraging;

        wxMessageBox( wxT("Open the light source"), wxT("Background"), wxOK | wxICON_INFORMATION);
//        MessageBox(NULL, "Open the laser", "Background", MB_OK);
    }

    int scanDirection = 1;
    if(monochromator.GetWavelengthStart() < monochromator.GetWavelengthStop())
        scanDirection = -1;

    int elements = (int) scanDirection * (monochromator.GetWavelengthStart() - monochromator.GetWavelengthStop()) / (double)monochromator.GetWavelengthStep();
    std::vector <int> vAccumulatedCounts(elements, 0);
    vWavelength.resize(elements);
    std::fill(vWavelength.begin(), vWavelength.end(), 0);
    vAveragedCounts.resize(elements);
    std::fill(vAveragedCounts.begin(), vAveragedCounts.end(), 0);

    for(int cycle = 0; cycle < monoCycles -> GetValue(); cycle++) // (тогда можно число циклов на ходу поменять)
    {

        for(int element = 0; element < elements; element++)
        {
            if (futureObj.wait_for(std::chrono::milliseconds(1)) != std::future_status::timeout)
            break;

            monochromator.GoToWavelength(monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep());

            counter.Start();
            usleep(counter.GetSampleTime());
            if(element == 0)
            vWavelength[element] = monochromator.GetWavelengthStart() + element * scanDirection*monochromator.GetWavelengthStep();
            vAccumulatedCounts[element] =+ counter.GetCount();

            vAveragedCounts[element] = (int)(vAccumulatedCounts[element] / (cycle + 1)) - background;


        }
        if (futureObj.wait_for(std::chrono::milliseconds(1)) != std::future_status::timeout)
        break;
    }
    monoStart -> SetLabel("Start");

}



void Framework_wx_pureFrame::AdjustmentStart(wxCommandEvent &event)
{

}
