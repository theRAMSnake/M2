package snakesoft.minion.materia

import android.util.Base64
import org.zeromq.ZMQ
import java.security.MessageDigest
import javax.crypto.Cipher
import javax.crypto.spec.IvParameterSpec
import javax.crypto.spec.SecretKeySpec

fun hashPassword(password: String): ByteArray {
    val digest = MessageDigest.getInstance("SHA-256")
    return digest.digest(password.toByteArray(Charsets.UTF_8))
}

fun encrypt(password: String, data: ByteArray): ByteArray {
    val secretKeySpec = SecretKeySpec(hashPassword(password), "AES")
    val iv = ByteArray(16)
    val charArray = password.toCharArray()
    for (i in 0 until iv.size){
        iv[i] = charArray[i].toByte()
    }
    val ivParameterSpec = IvParameterSpec(iv)

    val cipher = Cipher.getInstance("AES/CBC/Padding")
    cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec, ivParameterSpec)

    return cipher.doFinal(data)
}

fun decrypt(password: String, data: ByteArray): ByteArray {
    val secretKeySpec = SecretKeySpec(hashPassword(password), "AES")
    val iv = ByteArray(16)
    val charArray = password.toCharArray()
    for (i in 0 until iv.size){
        iv[i] = charArray[i].toByte()
    }
    val ivParameterSpec = IvParameterSpec(iv)

    val cipher = Cipher.getInstance("AES/CBC/Padding")
    cipher.init(Cipher.DECRYPT_MODE, secretKeySpec, ivParameterSpec)

    return cipher.doFinal(data)
}

//Encapsulates ZMQ communication with materia server
class MateriaConnection(Ip: String, val Password: String)
{
    private var mSocket: ZMQ.Socket
    private var mContext: ZMQ.Context = ZMQ.context(1)

    init
    {
        mSocket = mContext.socket(ZMQ.REQ)
        mSocket.connect("tcp://$Ip:5757")
        mSocket.receiveTimeOut = 30000
    }

    @Throws(MateriaUnreachableException::class)
    internal fun sendMessage(payload: com.google.protobuf.ByteString, serviceName: String, operationName: String): com.google.protobuf.ByteString
    {
        val toSend = common.Common.MateriaMessage.newBuilder()
                .setFrom("minion")
                .setTo(serviceName)
                .setOperationName(operationName)
                .setPayload(payload)
                .build()

        mSocket.send(encrypt(Password, toSend.toByteArray()))
        val response = mSocket.recv() ?: throw MateriaUnreachableException()

        val result = common.Common.MateriaMessage.parseFrom(decrypt(Password, response))
        return result.payload
    }
}
