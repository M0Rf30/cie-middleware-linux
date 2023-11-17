

// SetGraphometricData(
//         PdfString("Aruba_Sign_Biometric_Data"), PdfString(szGraphometricData),
//         PdfString(szVersion));


PoDoFo::PdfSignatureField::SetGraphometricData(PoDoFo::PdfString const&,
PoDoFo::PdfString const&,
PoDoFo::PdfString const&)
void method.PoDoFo::PdfSignatureField.SetGraphometricData_PoDoFo::PdfString_const___PoDoFo::PdfString_const___PoDoFo::PdfString_const
               (int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4)
{
    char cVar1;
    PdfDictionary& dict;
    int64_t string;
    undefined8 in_R9;
    int64_t in_FS_OFFSET;
    int64_t var_118h;
    int64_t var_110h;
    int64_t var_108h;
    int64_t var_100h;
    int64_t var_f8h;
    int64_t var_a8h;
    int64_t len;
    int64_t var_20h;
    
    // PoDoFo::PdfSignatureField::SetGraphometricData(PoDoFo::PdfString const&, PoDoFo::PdfString const&,
    // PoDoFo::PdfString const&)
    var_20h = *(int64_t *)(in_FS_OFFSET + 0x28);
    if (*(int64_t *)(arg1 + 0x20) == 0) {
        dict = __cxa_allocate_exception(0x60);
        var_a8h._0_4_ = 2;
        _ZN6PoDoFo8PdfErrorC1ERKNS_9EPdfErrorEPKciS5_
                  (dict, &var_a8h, "/home/piero/Desktop/IPZS/podofo-0.9.1/src/doc/PdfSignatureField.cpp", 0x145, 0, 
                   in_R9, arg4, arg3);
        __cxa_throw(dict, 0x4ce8df8948c68948, 0x58b48c28948);
    }
    
    dict = method.PoDoFo::PdfVariant.GetDictionary(*(int64_t *)(arg1 + 0x20));
    string = method.PoDoFo::PdfString.GetString___const(arg2);
    
    PoDoFo::PdfName::PdfName(char const*)((int64_t)&len, string);
    
    cVar1 = _ZNK6PoDoFo13PdfDictionary6HasKeyERKNS_7PdfNameE(dict, &len);
    _ZN6PoDoFo7PdfNameD1Ev(&len);

    if (cVar1 != '\0') {
        dict = method.PoDoFo::PdfVariant.GetDictionary(*(int64_t *)(arg1 + 0x20));
        string = method.PoDoFo::PdfString.GetString___const(arg2);
        PoDoFo::PdfName::PdfName(char const*)((int64_t)&len, string);
        _ZN6PoDoFo13PdfDictionary9RemoveKeyERKNS_7PdfNameE(dict, &len);
        _ZN6PoDoFo7PdfNameD1Ev(&len);
    }


    _ZN6PoDoFo13PdfDictionaryC1Ev(&var_f8h);
    _ZN6PoDoFo9PdfObjectC1ERKNS_9PdfStringE(&var_a8h, arg3);

    PoDoFo::PdfName::PdfName(char const*)((int64_t)&len, 0x8003fcc);
    _ZN6PoDoFo13PdfDictionary6AddKeyERKNS_7PdfNameERKNS_9PdfObjectE(&var_f8h, &len, &var_a8h);
    _ZN6PoDoFo7PdfNameD1Ev(&len);
    _ZN6PoDoFo9PdfObjectD1Ev(&var_a8h);
    _ZN6PoDoFo9PdfObjectC1ERKNS_9PdfStringE(&var_a8h, arg4);

    PoDoFo::PdfName::PdfName(char const*)((int64_t)&len, 0x8003fe8);
    _ZN6PoDoFo13PdfDictionary6AddKeyERKNS_7PdfNameERKNS_9PdfObjectE(&var_f8h, &len, &var_a8h);
    _ZN6PoDoFo7PdfNameD1Ev(&len);
    _ZN6PoDoFo9PdfObjectD1Ev(&var_a8h);

    dict = method.PoDoFo::PdfVariant.GetDictionary(*(int64_t *)(arg1 + 0x20));
    _ZN6PoDoFo9PdfObjectC1ERKNS_13PdfDictionaryE(&var_a8h, &var_f8h);
    
    string = method.PoDoFo::PdfString.GetString___const(arg2);
    
    PoDoFo::PdfName::PdfName(char const*)((int64_t)&len, string);
    _ZN6PoDoFo13PdfDictionary6AddKeyERKNS_7PdfNameERKNS_9PdfObjectE(dict, &len, &var_a8h);
    _ZN6PoDoFo7PdfNameD1Ev(&len);
    _ZN6PoDoFo9PdfObjectD1Ev(&var_a8h);
    _ZN6PoDoFo13PdfDictionaryD1Ev(&var_f8h);


    if (var_20h != *(int64_t *)(in_FS_OFFSET + 0x28)) {
        __stack_chk_fail();
    }
    return;
}