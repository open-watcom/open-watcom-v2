
#ifndef wmdichild_class
#define wmdichild_class

WCLASS WMdiWindow;
WCLASS WPopupMenu;
WCLASS WMdiChild : public WWindow
{
	public:
		WEXPORT WMdiChild( WMdiWindow* parent, char* text );
		WEXPORT ~WMdiChild();
		bool WEXPORT processMsg( UINT msg, UINT wparm, LONG lparm );
		void WEXPORT makeActive();
		virtual void WEXPORT activate( bool /*active*/ ) {}
		void WEXPORT close();
		void WEXPORT insertPopup( WPopupMenu* pop, int index );
		void WEXPORT removePopup( WPopupMenu* pop );
		static bool WEXPORT registerClass();
	private:
		static char* WEXPORT _childName;
};

#endif //wmdichild_class

