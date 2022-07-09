#include <utility>

#include "CardTemplate.h"
#include "../Util/util.h"
#include "../Util/ModuleInfo.h"
#include "CIEP11Template.h"

extern CLog Log;

extern CModuleInfo moduleInfo;

namespace p11 {

TemplateVector CCardTemplate::g_mCardTemplates;

CCardTemplate::CCardTemplate(void) {
}

CCardTemplate::~CCardTemplate(void) {
}

void CCardTemplate::AddTemplate(std::shared_ptr<CCardTemplate> pTemplate) {
    init_func
    g_mCardTemplates.emplace_back(std::move(pTemplate));
}

void CCardTemplate::DeleteTemplateList() {
    init_func
    g_mCardTemplates.clear();
}

void CCardTemplate::InitTemplateList() {
    init_func

    auto pTemplate = std::unique_ptr<CCardTemplate>(new CCardTemplate());
    pTemplate->szName = "CIE";  // "Carta d'Identità Elettronica";
    pTemplate->szManifacturer = "";
    pTemplate->FunctionList.templateInitLibrary = CIEtemplateInitLibrary;
    pTemplate->FunctionList.templateInitCard = CIEtemplateInitCard;
    pTemplate->FunctionList.templateFinalCard = CIEtemplateFinalCard;
    pTemplate->FunctionList.templateInitSession = CIEtemplateInitSession;
    pTemplate->FunctionList.templateFinalSession = CIEtemplateFinalSession;
    pTemplate->FunctionList.templateMatchCard = CIEtemplateMatchCard;
    pTemplate->FunctionList.templateGetSerial = CIEtemplateGetSerial;
    pTemplate->FunctionList.templateGetModel = CIEtemplateGetModel;
    pTemplate->FunctionList.templateLogin = CIEtemplateLogin;
    pTemplate->FunctionList.templateLogout = CIEtemplateLogout;
    pTemplate->FunctionList.templateReadObjectAttributes =
    CIEtemplateReadObjectAttributes;
    pTemplate->FunctionList.templateSign = CIEtemplateSign;
    pTemplate->FunctionList.templateSignRecover = CIEtemplateSignRecover;
    pTemplate->FunctionList.templateDecrypt = CIEtemplateDecrypt;
    pTemplate->FunctionList.templateGenerateRandom = CIEtemplateGenerateRandom;
    pTemplate->FunctionList.templateInitPIN = CIEtemplateInitPIN;
    pTemplate->FunctionList.templateSetPIN = CIEtemplateSetPIN;
    pTemplate->FunctionList.templateGetObjectSize = CIEtemplateGetObjectSize;
    pTemplate->FunctionList.templateSetKeyPIN = CIEtemplateSetKeyPIN;
    pTemplate->FunctionList.templateSetAttribute = CIEtemplateSetAttribute;
    pTemplate->FunctionList.templateCreateObject = CIEtemplateCreateObject;
    pTemplate->FunctionList.templateDestroyObject = CIEtemplateDestroyObject;
    pTemplate->FunctionList.templateGetTokenFlags = CIEtemplateGetTokenFlags;
    pTemplate->FunctionList.templateGenerateKey = CIEtemplateGenerateKey;
    pTemplate->FunctionList.templateGenerateKeyPair =
    CIEtemplateGenerateKeyPair;

    AddTemplate(std::move(pTemplate));
}

std::shared_ptr<CCardTemplate> CCardTemplate::GetTemplate(CSlot &pSlot) {
    init_func
    for (DWORD i=0; i < g_mCardTemplates.size(); i++) {
        try {
            if (g_mCardTemplates[i]->FunctionList.templateMatchCard(pSlot)) {
                return g_mCardTemplates[i];
            }
        } catch(...) { }
    }
    return nullptr;
}
};  // namespace p11
