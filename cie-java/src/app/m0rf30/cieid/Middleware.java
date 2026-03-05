package app.m0rf30.cieid;

import com.sun.jna.Callback;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;
import java.util.ArrayList;
import java.util.List;

public interface Middleware extends Library {
    Middleware INSTANCE = Native.load("opencie-pkcs11", Middleware.class);
    int MAX_LEN = 512; // Updated to match OPENCIE_MAX_LEN
    int MAX_INFO = 20;

    interface ProgressCallBack extends Callback {
        void invoke(int progress, String message);
    }

    interface CompletedCallBack extends Callback {
        void invoke(String pan, String name, String ef_seriale);
    }

    interface SignCompletedCallBack extends Callback {
        void invoke(int retValue);
    }

    class verifyInfo extends Structure {
        public static class ByReference extends verifyInfo implements Structure.ByReference {}

        public byte[] name = new byte[Middleware.MAX_LEN * 2];
        public byte[] surname = new byte[Middleware.MAX_LEN * 2];
        public byte[] cn = new byte[Middleware.MAX_LEN * 2];
        public byte[] signingTime = new byte[Middleware.MAX_LEN * 2];
        public byte[] cadn = new byte[Middleware.MAX_LEN * 2];
        public int CertRevocStatus;
        public byte isSignValid;
        public byte isCertValid;

        @Override
        protected List<String> getFieldOrder() {
            List<String> fields = new ArrayList<String>();
            fields.add("name");
            fields.add("surname");
            fields.add("cn");
            fields.add("signingTime");
            fields.add("cadn");
            fields.add("CertRevocStatus");
            fields.add("isSignValid");
            fields.add("isCertValid");

            return fields;
        }

        private String byteArrayToString(byte[] byteArray) {
            return new String(byteArray);
        }

        public String get_name() {
            return byteArrayToString(name);
        }

        public String get_surname() {
            return byteArrayToString(surname);
        }

        public String get_cn() {
            return byteArrayToString(cn);
        }

        public String get_signingTime() {
            if (signingTime[0] != 0) {
                return byteArrayToString(signingTime);
            }

            return null;
        }

        public String get_cadn() {
            return byteArrayToString(cadn);
        }

        public void printVerifyInfo() {
            System.out.println("Name: " + get_name());
            System.out.println("Surname: " + get_surname());
            System.out.println("Cn: " + get_cn());
            System.out.println("Signing Time: " + get_signingTime());
            System.out.println("Cadn: " + get_cadn());
            System.out.println("Is sign valid: " + isSignValid);
            System.out.println("Is cert valid: " + isCertValid);
            System.out.println("Cert revoc status: " + CertRevocStatus);
        }
    }

    // Updated function name: AbilitaCIE -> cie_enable
    int cie_enable(
            String szPAN,
            String szPIN,
            int[] attempts,
            ProgressCallBack progressCallBack,
            CompletedCallBack completedCallBack);

    // Updated function name: VerificaCIEAbilitata -> cie_is_enabled
    int cie_is_enabled(String szPAN);

    // Updated function name: firmaConCIE -> cie_sign
    int cie_sign(
            String inFilePath,
            String type,
            String pin,
            String pan,
            int page,
            float x,
            float y,
            float w,
            float h,
            String imagePathFile,
            String outFilePath,
            ProgressCallBack progressCallBack,
            SignCompletedCallBack completedCallBack);

    // Updated function name: DisabilitaCIE -> cie_disable
    int cie_disable(String szPAN);

    // Updated function name: CambioPIN -> cie_change_pin
    int cie_change_pin(
            String currentPIN, String nuovoPIN, int[] attempts, ProgressCallBack progressCallBack);

    // Updated function name: SbloccoPIN -> cie_unblock_pin
    int cie_unblock_pin(
            String puk, String nuovoPIN, int[] attempts, ProgressCallBack progressCallBack);

    // Updated function name: verificaConCIE -> cie_verify
    int cie_verify(String inFilePath, String proxyAddress, int proxyPort, String usrPass);

    // Updated function name: getNumberOfSign -> cie_get_sign_count
    int cie_get_sign_count();

    // Updated function name: getVerifyInfo -> cie_get_verify_info
    int cie_get_verify_info(int index, verifyInfo vInfo);

    // Updated function name: estraiP7m -> cie_extract_p7m
    int cie_extract_p7m(String inFilePath, String outFilePath);
}
