//Defines.h

#define WMU_FIRST							WM_USER								+ 1024
#define WMU_STATUS						WMU_FIRST
#define WMU_OUTPUT						WMU_STATUS						+ 1
#define WMU_URL								WMU_OUTPUT						+ 1
#define WMU_PORT_LOCAL				WMU_URL								+ 1
#define WMU_PORT_REMOTE 			WMU_PORT_LOCAL				+ 1
#define WMU_LISTEN						WMU_PORT_REMOTE				+ 1
#define WMU_DLG_PORT_SCANNER	WMU_LISTEN						+ 1
#define WMU_MENU							WMU_DLG_PORT_SCANNER	+ 1
#define WMU_UPDATE_TITLE			WMU_MENU							+ 1
#define WMU_HELP_FINDER				WMU_UPDATE_TITLE			+ 1
#define WMU_BANNER						WMU_HELP_FINDER				+ 1
#define WMU_REFRESH_BANNER		WMU_BANNER						+ 1
#define WMU_BANNER_URL        WMU_REFRESH_BANNER		+ 1
#define WMU_CONTEXT_MENU			WMU_BANNER_URL        + 1

#define WMU_LAST              WMU_CONTEXT_MENU				+ 1

