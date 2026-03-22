#ifdef __cplusplus
extern "C" {
#endif
int assetbundler_bundle(const char* input_path, const char* output_path);
int assetbundler_add_asset(const char* path);
void assetbundler_clear();
#ifdef __cplusplus
}
#endif
