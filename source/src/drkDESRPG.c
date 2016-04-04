#ifdef __GNUC__
#include <PalmOS.h>    // for PRC Tool compatability
#include "stdio.h"
#endif
#define MainForm        1000
#define OptionsMenuAbout 1000
#define AboutAlert 1000
#define LISTE_DES 1300
#define BOUTON_JET 1200
#define CHAMP_TOTAL 1101
#define BOUTON_RAZ 1201
#define LISTE_RESULTAT 1102

// *** PUT UI-DEFINITIONS HERE *** //
MemHandle ListeDes;
int Total = 0;
 
// Prototypes
static Boolean appHandleEvent (EventPtr pEvent);
static void    mainFormInit (FormPtr pForm);
static Boolean mainFormEventHandler (EventPtr pEvent);
static Boolean doMainMenu (FormPtr pForm, UInt16 command);
static void *getObjectPtr (FormPtr pForm, Int16 resourceNo);
static int tireDes(int nbF);
static int getNbFaces(int sel);
void jeterLesDes(FormPtr pForm);
void raz(FormPtr pForm);
void afficheResultats(FormPtr pForm);

// Code
static void startApp() {return;}
static void stopApp()  {return;}

UInt32 PilotMain (UInt16 cmd,void *cmdPBP,UInt16 launchFlags) {
    EventType    event;
    UInt16       error;
    if (cmd == sysAppLaunchCmdNormalLaunch)
    {
        startApp();
        FrmGotoForm(MainForm);
        do
        {
            // Wait for an event
            EvtGetEvent(&event, evtWaitForever);
            // ask event handlers, in turn, to deal with the event.
            if (!SysHandleEvent (&event))
            if (!MenuHandleEvent (0, &event, &error))
            if (!appHandleEvent (&event))
                FrmDispatchEvent (&event);
        } while (event.eType != appStopEvent);
        stopApp();
        FrmCloseAllForms();
    }
    return 0;
}

/*
 * top-level event handler for the application.
 */

static Boolean appHandleEvent (EventPtr pEvent) {
    FormPtr pForm;
    Int16   formId;
    Boolean handled = false;
    if (pEvent->eType == frmLoadEvent)
    {
        // Load the resource for the form
        formId = pEvent->data.frmLoad.formID;
        pForm  = FrmInitForm(formId);
        FrmSetActiveForm(pForm);
        // install a form-specific event handler
        if (formId == MainForm)
            FrmSetEventHandler (pForm, mainFormEventHandler);
        // *** ADD NEW FORM HANDLING HERE *** //
        handled = true;
    }
    else if (pEvent->eType == menuEvent)
    {
        handled = doMainMenu(pForm, pEvent->data.menu.itemID);
    }
    return handled;
}

/*
 * Event handler for the main form.
 */
static Boolean mainFormEventHandler(EventPtr pEvent) {
    Boolean handled = false;
    FormPtr pForm   = FrmGetActiveForm();
    switch (pEvent->eType)
    {
      case frmOpenEvent:    // Form's 1st event
          FrmDrawForm(pForm);
          mainFormInit(pForm);
          handled = true;
          break;
      // *** ADD EVENT HANDLING HERE *** //
		  case menuEvent:
			  handled = doMainMenu(pForm, pEvent->data.menu.itemID);
			  break;
		  case ctlSelectEvent : {
			  switch (pEvent->data.ctlSelect.controlID) {
				  case BOUTON_JET :
				      jeterLesDes(pForm);
					  handled = true;
					  break;
				  case BOUTON_RAZ :
				      raz(pForm);
					  handled = true;
					  break;
			  }
		  }
      default:
        break;
    }
    return handled;
}

/*
 * This is the menu handler for the main form.
 */
static Boolean doMainMenu(FormPtr pForm, UInt16 command) {
    Boolean    handled    = false;
    switch(command)
    {
		  case OptionsMenuAbout :
			  FrmAlert(AboutAlert);
			  handled = true;
			  break;
    }
    return handled;
}
/*
 * Startup code for the form.
 */
static void mainFormInit(FormPtr pForm) {
	FormPtr frmP = FrmGetActiveForm();
	// RAZ
	 ListeDes=MemHandleNew(sizeof(char) * 1000);
	raz(frmP);
    // On sélectionne par défaut des Dés à 6 Faces
	ListPtr lstP = getObjectPtr(frmP, LISTE_DES);
	LstSetSelection(lstP,2);
	
	FrmDrawForm(frmP);
}

void *getObjectPtr (FormPtr pForm, Int16 resourceNo) {
    UInt16 objIndex=FrmGetObjectIndex(pForm,resourceNo);
    return FrmGetObjectPtr(pForm,objIndex);
}

int tireDes(int nbF){
  return ((SysRandom(0)%nbF)+1);
}

int getNbFaces(int sel) {
    if (sel==0) return(3);
	else if (sel==1) return(4);
	else if (sel==2) return(6);
	else if (sel==3) return(8);
	else if (sel==4) return(10);
	else if (sel==5) return(12);
	else if (sel==6) return(20);
	else if (sel==7) return(100);
	else return(6);
}

void jeterLesDes(FormPtr pForm){
 	FormPtr frmP = FrmGetActiveForm();

	// Recuperer le nombre de Faces
	ListPtr lstP = getObjectPtr(frmP, LISTE_DES);
	int nbF = getNbFaces(LstGetSelection(lstP));	
	
	// On lance une boucle pour lancer les des
	char *l=(char *)MemHandleLock(ListeDes);
	MemHandle UnDes=MemHandleNew(sizeof(char) * 6);
	char *ud=(char *)MemHandleLock(UnDes);
 	//StrCopy(l,"\0");
	int d=0;
    d=tireDes(nbF);
    StrIToA(ud,d);
    StrCat(l,"[");
	StrCat(l,ud);
	StrCat(l,"]\n");
	Total+=d;
    MemHandleUnlock(ListeDes);
    MemHandleUnlock(UnDes);
    //Afficher les resultats
    afficheResultats(frmP);
}

void raz(FormPtr pForm) {
	Total=0;
	char *l=(char *)MemHandleLock(ListeDes);
 	StrCopy(l,"\0");	
    MemHandleUnlock(ListeDes);
    afficheResultats(pForm);
}

void afficheResultats(FormPtr frmP) {
	// Renseigner la liste des Des
	FieldPtr fldP = getObjectPtr(frmP, LISTE_RESULTAT);
	FldSetTextHandle(fldP,ListeDes);
	FldDrawField(fldP);
	// Renseigner le Total
	FieldPtr fldP1 = getObjectPtr(frmP, CHAMP_TOTAL);
    MemHandle h;
    h=MemHandleNew(sizeof(char) * 10);
    StrIToA((char *)MemHandleLock(h),Total);
	MemHandleUnlock(h);
	FldSetTextHandle(fldP1,h);
	FldDrawField(fldP1);
}

