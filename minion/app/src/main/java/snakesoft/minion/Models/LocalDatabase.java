package snakesoft.minion.Models;

import android.content.Context;

import com.google.common.io.Files;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;

/**
 * Created by snake on 4/30/17.
 */

public class LocalDatabase
{
    public LocalDatabase(Context context)
    {
        mContext = context;
    }

    public byte[] get(String path)
    {
        try
        {
            RandomAccessFile f = new RandomAccessFile(mContext.getApplicationInfo().dataDir + "/" + path, "r");
            byte[] b = new byte[(int)f.length()];
            f.readFully(b);
            return b;
        }
        catch(FileNotFoundException ex)
        {

        }
        catch(IOException ex)
        {

        }
        return null;
    }

    public void put(String path, byte[] data)
    {
        try
        {
            Files.write(data, new File(mContext.getApplicationInfo().dataDir + "/" + path));
        }
        catch (IOException e)
        {

        }
    }

    private Context mContext;
}
