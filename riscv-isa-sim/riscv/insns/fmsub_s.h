require_either_extension('F', EXT_ZFINX);
require_fp;
softfloat_roundingMode = RM;
WRITE_FRD_F(f32_mulAdd(FRS1_F, FRS2_F, f32(FRS3_F.v ^ F32_SIGN)));
set_fp_exceptions;
