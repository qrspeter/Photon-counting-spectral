/***************************************************************
 * Name:      Framework_wx_pureApp.h
 * Purpose:   Defines Application Class
 * Author:    Peter ()
 * Created:   2021-11-29
 * Copyright: Peter ()
 * License:
 **************************************************************/

#ifndef FRAMEWORK_WX_PUREAPP_H
#define FRAMEWORK_WX_PUREAPP_H

#include <wx/app.h>



class Framework_wx_pureApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // FRAMEWORK_WX_PUREAPP_H
