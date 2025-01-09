/**
 * 
 */

/**
 * @author jcamposp
 *
 */
public class Prova {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// 
		String texto = "Operación Fusión FUSIÓN OPERACIÓN";
		System.out.println(removeTildes(texto));
	}
	
	private static String removeTildes(String texto) {
	    String original = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ";
	    // Cadena de caracteres ASCII que reemplazarán los originales.
	    String ascii = "AAAAAAACEEEEIIIIDNOOOOOOUUUUYBaaaaaaaceeeeiiiionoooooouuuuyy";
	    String output = texto;
	    for (int i=0; i<original.length(); i++) {
	    // Reemplazamos los caracteres especiales.
	        output = output.replace(original.charAt(i), ascii.charAt(i));
	    }
	    return output;
	}//for i

}
