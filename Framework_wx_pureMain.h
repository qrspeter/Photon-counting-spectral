/***************************************************************
 * Name:      Framework_wx_pureMain.h
 * Purpose:   Defines Application Frame
 * Author:    Peter ()
 * Created:   2021-11-29
 * Copyright: Peter ()
 * License:
 **************************************************************/

#ifndef FRAMEWORK_WX_PUREMAIN_H
#define FRAMEWORK_WX_PUREMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "Framework_wx_pureApp.h"
#include "mathplot/mathplot.h"


#include <wx/spinctrl.h> // for wxSpinCtrl

class Framework_wx_pureFrame: public wxFrame
{
    public:
        Framework_wx_pureFrame(wxFrame *frame, const wxString& title);
        ~Framework_wx_pureFrame();
    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout,
            idMenuSaveAs,
            idMenuSettingSave,
            idMenuSettingLoad,
            idMenuSettingDefault,

            idCounterCOM,
            idCounterConnect,
            idCounterSlope,
            idCounterDiscr,
            idCounterSampleTime,
            idCounterDeadTime,

            idMonoConnect,
            idMonoCOM,
            idMonoGrate,
            idMonoWavelengthStart,
            idMonoWavelengthStop,
            idMonoWavelengthStep,
            idMonoStart,
            idAdjustmentStart,
            idMonoStop,
            idAdjustmentStop,
            // sync
            idSyncMode,
            idSyncConnect,

        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSaveAs(wxCommandEvent& event);
        void onSettingSave(wxCommandEvent& event);
        void onSettingDefault(wxCommandEvent& event);

        void CounterCOM(wxCommandEvent &event);
        void CounterConnect(wxCommandEvent &event);
        void CounterSlope(wxCommandEvent &event);
        void CounterDiscr(wxCommandEvent &event);
        void CounterSampleTime(wxCommandEvent &event);
        void CounterDeadTime(wxCommandEvent &event);

        void MonoConnect(wxCommandEvent &event);
        void MonoCOM(wxCommandEvent &event);
        void MonoGrate(wxCommandEvent &event);
        void MonoWavelengthStart(wxCommandEvent &event);
        void MonoWavelengthStop(wxCommandEvent &event);
        void MonoWavelengthStep(wxCommandEvent &event);
        void AdjustmentStart(wxCommandEvent &event);
        void MonoStart(wxCommandEvent &event);
        void AdjustmentStop(wxCommandEvent &event);
        void MonoStop(wxCommandEvent &event);
        int Background();
        int Count();
        int Measurement();

        void SyncMode(wxCommandEvent &event);
        void SyncConnect(wxCommandEvent &event);

        wxPanel *framework_panel;
        wxBoxSizer *framework_sizer;

        wxChoice *counterComChoice;
        wxButton *counterComConnect;
        wxChoice *counterSlope;
        wxSpinCtrl *counterDiscr;
        wxSpinCtrl *counterSampleTime;
        wxSpinCtrl *counterDeadTime;

        wxChoice *monoComChoice;
        wxButton *monoComConnect;
        wxChoice *monoGrate;
        wxSpinCtrl *monoWavelengthStart;
        wxSpinCtrl *monoWavelengthStop;
        wxSpinCtrl *monoWavelengthStep;
        wxSpinCtrl *monoCycles;
        wxCheckBox *monoBackground;
        wxButton *monoStart;
        wxButton *monoStop;
        wxStaticText *cyclesIndicator;

        // sync
        wxCheckBox *syncMode;
        wxGridSizer *wxSyncGrid;
        wxChoice *syncComChoice;
        wxButton *syncComConnect;
        wxSpinCtrl *syncCycles;

        wxCheckBox *adjustmentBackground;
        wxSpinCtrl *adjustmentWavelength;
        wxButton *adjustmentStart;
        wxButton *adjustmentStop;


        mpWindow *framework_graph;
 /*       static mpFXYVector *frameworkVector;
        static mpScaleY *scaleY;
        static mpScaleX *scaleX;
        static mpInfoCoords *frame_coord;
*/
        DECLARE_EVENT_TABLE()

        //const wxArrayString  monocromatorPorts;


};


#endif // FRAMEWORK_WX_PUREMAIN_H
