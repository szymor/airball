

enum
{
	e_EditorMode_Basic = 0,
	e_EditorMode_Map,			// Mode map.
};

struct	SEditor
{
	u8	nMode;					// Basic / Map.

	s32	nPosX, nPosY, nPosZ;	// Position du bloc d'édition en cours.
	u32	nBlk;					// Bloc à poser.

	s32	nMouseArrowsZone;		// -1, pas dans une zone de clic.

};
extern struct	SEditor	gEditor;


void Editor_Init(void);
void Editor_SelBlk_Show(void);
void Editor_SelBlk_PosReset(void);
void Editor_ModeDisp(void);
void Editor_ArrowPressed(u32 nDir);
void Editor_Keys(void);

void Editor_FileLoad(char *pFilename);
void Editor_FileSave(char *pFilename);

void Mouse_ZoneDisp(void);
void Mouse_ZoneClick(void);

