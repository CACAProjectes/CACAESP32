package es.xuan.proves;

public class Serial {

	public static void main(String[] args) {
		int CTE_MIN_BUCLE = 1;
		int CTE_MAX_BUCLE = 10;
		
		boolean bIntermitenteIzq = true;
		boolean bIntermitenteDer = false;
		boolean bSirena = true;
		boolean bPosicion = false;
		boolean bCruce = false;
		boolean bCarretera = false;
		
		int iRespuesta = 0b00000000;
		
		int CTE_IntermDerON = 0b10000000;	// U1 - QA
		int CTE_IntermIzqON = 0b01000000;	// U1 - QB
		int CTE_SirenaBON  	= 0b00100000;	// U1 - QC
		int CTE_SirenaGON  	= 0b00010000;	// U1 - QD
		int CTE_SirenaRON  	= 0b00001000;	// U1 - QE

		for (int j=0;j<2;j++) {
			System.out.println("Bucle: " + j); 
			for (int iContador=CTE_MIN_BUCLE;iContador<=CTE_MAX_BUCLE;iContador++) {
				////////////////
				// Intermitentes
				////////////////
				if (bIntermitenteIzq && iContador <= CTE_MAX_BUCLE/2) 
					// Intermitente Izq ON
					iRespuesta = iRespuesta | CTE_IntermIzqON;	
				if (bIntermitenteIzq && iContador > CTE_MAX_BUCLE/2) 
					// Intermitente Izq OFF
					iRespuesta = iRespuesta & ~CTE_IntermIzqON;	
				if (bIntermitenteDer && iContador <= CTE_MAX_BUCLE/2) 
					// Intermitente Der ON
					iRespuesta = iRespuesta | CTE_IntermDerON;
				if (bIntermitenteDer && iContador > CTE_MAX_BUCLE/2) 
					// Intermitente Der OFF
					iRespuesta = iRespuesta & ~CTE_IntermDerON;	
				////////////////
				// Sirena
				////////////////
				if (bSirena && iContador >= CTE_MIN_BUCLE && iContador <= CTE_MIN_BUCLE + 2) {
					// Sirena B
					iRespuesta = iRespuesta | CTE_SirenaBON;	
					iRespuesta = iRespuesta & ~CTE_SirenaRON;	
				}
				if (bSirena && iContador == CTE_MIN_BUCLE + 3) {
					// Sirena R
					iRespuesta = iRespuesta & ~CTE_SirenaBON;	
					iRespuesta = iRespuesta | CTE_SirenaRON;	
				}
				if (bSirena && iContador >= CTE_MIN_BUCLE + 4 && iContador <= CTE_MIN_BUCLE + 6) {
					// Sirena B
					iRespuesta = iRespuesta | CTE_SirenaBON;	
					iRespuesta = iRespuesta & ~CTE_SirenaRON;	
				}
				if (bSirena && iContador >= CTE_MIN_BUCLE + 7 && iContador <= CTE_MIN_BUCLE + 9) {
					// Sirena R
					iRespuesta = iRespuesta & ~CTE_SirenaBON;	
					iRespuesta = iRespuesta | CTE_SirenaRON;	
				}
				//
				if (!bSirena) {
					// Sirena OFF
					iRespuesta = iRespuesta & ~CTE_SirenaBON;	
					iRespuesta = iRespuesta & ~CTE_SirenaRON;				
				}
				if (!bIntermitenteIzq) {
					iRespuesta = iRespuesta & ~CTE_IntermIzqON;
				}
				if (!bIntermitenteDer) {
					iRespuesta = iRespuesta & ~CTE_IntermDerON;
				}
				//
				String binario = Integer.toBinaryString(iRespuesta);
				System.out.println(String.format("%8s" , binario).replaceAll(" ", "0")); 
			}
		}
	}
}
