/* ../intel/c/i86segs.c */
extern hw_reg_set CalcSegment(cg_sym_handle sym, cg_class class);
extern name *AddrConst(name *value, segment_id seg, constant_class class);
extern bool SegIsCS(name *op);
extern bool SegIsSS(name *op);
extern name *GetSegment(name *op);
extern name *NearSegment(void);
extern name *SegName(name *op);
extern bool SegOver(name *op);
