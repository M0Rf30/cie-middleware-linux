package app.m0rf30.cieid;

import com.sun.jna.Callback;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;
import java.util.ArrayList;
import java.util.List;

public interface Middleware extends Library {
    Middleware INSTANCE = Native.load("cie-pkcs11", Middleware.class);
    int MAX_LEN = 256;
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

    int AbilitaCIE(
            String szPAN,
            String szPIN,
            int[] attempts,
            ProgressCallBack progressCallBack,
            CompletedCallBack completedCallBack);

    int VerificaCIEAbilitata(String szPAN);

    int firmaConCIE(
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

    int DisabilitaCIE(String szPAN);

    int CambioPIN(
            String currentPIN, String nuovoPIN, int[] attempts, ProgressCallBack progressCallBack);

    int SbloccoPIN(String puk, String nuovoPIN, int[] attempts, ProgressCallBack progressCallBack);

    int verificaConCIE(String inFilePath, String proxyAddress, int proxyPort, String usrPass);

    int getNumberOfSign();

    int getVerifyInfo(long index, verifyInfo vInfo);

    int estraiP7m(String inFilePath, String outFilePath);
}
