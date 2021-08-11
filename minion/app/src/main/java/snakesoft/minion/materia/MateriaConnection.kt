package snakesoft.minion.materia

import android.util.Base64
import android.util.Base64.*
import okhttp3.MediaType
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.IOException
import java.nio.charset.Charset
import java.security.MessageDigest
import javax.crypto.Cipher
import javax.crypto.spec.IvParameterSpec
import javax.crypto.spec.SecretKeySpec


fun ByteArray.toHexString() : String {
    return this.joinToString("") {
        java.lang.String.format("%02x", it)
    }
}

fun hashPassword(password: String): ByteArray {
    val digest = MessageDigest.getInstance("SHA-256")
    return digest.digest(password.toByteArray(Charsets.UTF_8))
}

fun encrypt(password: String, data: ByteArray): ByteArray {
    val hashed = hashPassword(password)

    println("Hashed is: ${hashed.toHexString()}, size ${hashed.size}")

    val secretKeySpec = SecretKeySpec(hashed, 0, 32, "AES")
    val ivParameterSpec = IvParameterSpec(hashed, 0, 16)

    val cipher = Cipher.getInstance("AES/CBC/PKCS5Padding")
    cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec, ivParameterSpec)

    return cipher.doFinal(data)
}

fun decrypt(password: String, data: ByteArray): ByteArray {
    val hashed = hashPassword(password)
    val secretKeySpec = SecretKeySpec(hashed, 0, 32,"AES")
    val ivParameterSpec = IvParameterSpec(hashed, 0, 16)

    val cipher = Cipher.getInstance("AES/CBC/PKCS5Padding")
    cipher.init(Cipher.DECRYPT_MODE, secretKeySpec, ivParameterSpec)

    return cipher.doFinal(data)
}

class MateriaConnection(var Ip: String, val Password: String, val Port: String)
{
    val client = OkHttpClient()
    val mediaType: MediaType = "text/html; charset=utf-8".toMediaType()

    @Throws(MateriaUnreachableException::class)
    internal fun sendMessage(payload: String): String
    {
        println("payload: $payload")
        val based = Base64.encodeToString(encrypt(Password, payload.toByteArray()), NO_WRAP)
        println("based: $based")
        val toSend = based.toRequestBody(mediaType)
        val request = Request.Builder()
                .url("http://ramsnake.net:5754/api")
                .addHeader("Content-Length", "${toSend.contentLength()}")
                .post(toSend)
                .build()

        var result = ""
        client.newCall(request).execute().use{ response ->
            if (!response.isSuccessful)
            {
                throw IOException("Unexpected code $response")
            }

            result = response.body!!.string()
        }

        val dc = decrypt(Password, Base64.decode(result, DEFAULT)).toString(Charset.defaultCharset())
        println("resp: $dc")
        return dc
    }
}
