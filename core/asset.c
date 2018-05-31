#include "asset.h"

void* getImg(char* name) {
	logger->inf("=== GETTING IMG: %s ===", name);
	AssetMgr* ast = getAssets();
	SDL_Surface* img = (SDL_Surface*) ast->isCached(name, ast->imgs);

	if (img == NULL){
		char imgPath[100];
		snprintf(imgPath, 100, "asset/%s.png", name);
		
		logger->dbg("-- Fething File: %s", imgPath);

		img = IMG_Load(imgPath);
		if (img == NULL){
			logger->err("-- Fail to get Image: %s", imgPath);
			logger->dbg("==== GETTING IMG FAILD ====");
			return NULL;
		}

		ast->cache(name, ast->imgs, img);
	}
	else{
		logger->dbg("-- isCached");
	}

	logger->dbg("=== GETTING IMG DONE ===", name);
	return img;
}

void* getFont(char* name, short size) {
	logger->inf("=== GETTING FONT: %s ===", name);
	AssetMgr* ast = getAssets();

	char fontName[100];
	snprintf(fontName, 88, "%s-%d", name, size);

	TTF_Font* font = (TTF_Font*) ast->isCached(fontName, ast->fonts);

	if (font == NULL){
		char fontPath[100];
		snprintf(fontPath, 100, "font/%s.ttf", name);
		
		logger->dbg("-- Fething File: %s", fontPath);
		
		font = TTF_OpenFont(fontPath, size);
		if (font == NULL){
			logger->err("-- Fail to get FONT: %s", fontPath);
			logger->dbg("==== GETTING FONT FAILD ====");
			return NULL;
		}

		ast->cache(fontName, ast->fonts, font);
	}
	else{
		logger->dbg("-- isCached");
	}

	logger->dbg("=== GETTING FONT DONE ===");
	return font;
}

void clearAssets() {
	logger->inf("=== CLEAR ASSETS ===");

	AssetMgr* ast = getAssets();
	ast->clearImgs();
	ast->clearFonts();

	logger->dbg("=== CLEAR ASSETS DONE ===");
}

void clearImgs() {
	logger->inf("=== CLEAR IMAGES ===");
	AssetMgr* ast = getAssets();

	if (ast->imgs == NULL) {
		logger->dbg("=== CLEAR IMAGES DONE ===");
		return;
	}

	Node* n = NULL;
	while((n = listIterate(ast->imgs, n)) != NULL) {
		logger->dbg("-- Clearing: %s", n->name);
		
		logger->dbg("-- Free Surface");
	    SDL_FreeSurface(n->value);

		logger->dbg("-- Delete Node");
	    deleteNode(ast->imgs, n->id);

	    n = NULL;
	}
	
	logger->dbg("=== CLEAR IMAGES DONE ===");
}

void clearFonts() {
	logger->inf("=== CLEAR FONTS ===");
	AssetMgr* ast = getAssets();

	if (ast->fonts == NULL) {
		logger->dbg("=== CLEAR FONTS DONE ===");
		return;
	}

	Node* n = NULL;
	while((n = listIterate(ast->fonts, n)) != NULL) {
		logger->dbg("-- Clearing: %s", n->name);
		
		logger->dbg("-- Free Font");
	    TTF_CloseFont(n->value);

		logger->dbg("-- DELETE NODE");
	    deleteNode(ast->fonts, n->id);

	    n = NULL;
	}
	
	logger->dbg("=== CLEAR FONTS DONE ===");
}

void* assetIsCached(char* name, ListManager* cont) {
	logger->dbg("-- Search Cache: %s", name);
	Node* n = getNodeByName(cont, name);
	
	if (n != NULL) {
		logger->dbg("-- Cache Found: %d", n->id);
		return n->value;
	}

	return NULL;
}

void* cacheAsset(char* name, ListManager* cont, void* data) {
	logger->dbg("-- Caching: %s", name);
	Node* n =  addNodeV(cont, name, data, 0);
	
	logger->dbg("-- Cache Node: %p", n);
	logger->dbg("-- id: %d", n->id);
	logger->dbg("-- name: %s", n->name);

	return n;
}

void destroyAssets() {
	logger->inf("=== DESTROY ASSESTS ===");
	AssetMgr* ast = getAssets();
	
	ast->clear();
	deleteList(ast->imgs);
	deleteList(ast->fonts);

	ast = NULL;
	logger->dbg("=== DESTROY ASSESTS ===");
}

AssetMgr* getAssets() {
	static AssetMgr* ast = NULL;

	if (ast != NULL) {
		return ast;
	}

	logger->inf("=== INIT ASSEST MANAGER ===");
	ast = malloc(sizeof(AssetMgr));
	ast->imgs = initListMgr();
	ast->fonts = initListMgr();

	ast->getImg = getImg;
	ast->getFont = getFont;
	ast->clear = clearAssets;
	ast->clearImgs = clearImgs;
	ast->clearFonts = clearFonts;

	ast->isCached = assetIsCached;
	ast->cache = cacheAsset;

	ast->destroy = destroyAssets;
	logger->dbg("=== INIT ASSEST DONE ===");

	return ast;
}