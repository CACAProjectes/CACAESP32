package es.xuan.appprueba;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class MainActivity extends AppCompatActivity {

    private TextView m_etTextoIn;
    private TextView m_etTextoOut;
    private TextView m_etTextoError;
    private Button m_btEnviar;
    //
    private static final String SERVER_ADDRESS = "192.168.4.1";
    private static final int SERVER_PORT = 80;
    private static String serverResponse = "";
    private static String clientMessage = "";
    //
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        //
        inicializar_pantalla();
        //
        //inicializar_cliente();
    }

    private void inicializar_pantalla() {
        m_etTextoIn = findViewById(R.id.etTextoEntrada);
        m_etTextoOut = findViewById(R.id.etTextoSalida);
        m_btEnviar = findViewById(R.id.btEnviar);
        //
        m_etTextoOut.setText(getString(R.string.mensaje_bienvenida));
        m_etTextoIn.requestFocus();
    }

    public void enviarMensaje(View view) {
        clientMessage = m_etTextoIn.getText().toString();
        escribir_log(clientMessage);
        m_etTextoIn.setText("");
        //
        m_etTextoIn.requestFocus();
        //
        try {
            inicializar_cliente();
        } catch (Throwable th) {
            escribir_error(th.getMessage());
        }
    }

    private void escribir_log(String pStrTexto) {
        String str = m_etTextoOut.getText() +
                "\r\n" +
                pStrTexto;
        m_etTextoOut.setText(str);
    }
    private void escribir_error(String pStrTexto) {
        String str = m_etTextoError.getText() +
                "\r\n" +
                pStrTexto;
        m_etTextoError.setText(str);
    }

    private void inicializar_cliente() {
        try {
            Socket socket = new Socket(SERVER_ADDRESS, SERVER_PORT);
            //System.out.println("Connected to the chat server!");
            escribir_log("Connected to the chat server!");

            // Setting up input and output streams
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            // Start a thread to handle incoming messages
            new Thread(() -> {
                try {
                    while ((serverResponse = in.readLine()) != null) {
                        //System.out.println(serverResponse);
                        escribir_log(serverResponse);
                    }
                } catch (IOException e) {
                    escribir_error(e.getMessage());
                }
            }).start();
            //  Enviar mensaje
            out.println(clientMessage);
        } catch (Throwable th) {
            escribir_error(th.getMessage());
        }
    }
}