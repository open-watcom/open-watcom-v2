/* Internal MP3 decoder interfaces */

extern FLOAT32  g_synth_n_win[64][32];
extern FLOAT32  v_vec[2 /* ch */][1024];
extern FLOAT32  g_synth_dtbl[512];
extern uint32_t g_huffman_main [34][3];

extern void     MPG_Read_Huffman( uint32_t part_2_start, uint32_t gr, uint32_t ch );
extern uint32_t MPG_Get_Byte( void );
extern int      MPG_Get_Bytes( uint32_t no_of_bytes, uint32_t data_vec[] );
extern uint32_t MPG_Get_Filepos( void );
