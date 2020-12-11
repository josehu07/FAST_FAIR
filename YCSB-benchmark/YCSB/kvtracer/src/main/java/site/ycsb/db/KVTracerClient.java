package site.ycsb.db;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.HashSet;
import java.util.SortedMap;
import java.util.Vector;
import java.util.concurrent.TimeUnit;

import java.io.File;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.IOException;
import java.io.BufferedWriter;
import java.io.BufferedReader;
import java.io.OutputStreamWriter;

import site.ycsb.ByteArrayByteIterator;
import site.ycsb.ByteIterator;
import site.ycsb.DB;
import site.ycsb.Status;
import site.ycsb.DBException;

/**
 * A database interface layer for generating key-value workload traces
 */
public class KVTracerClient extends DB {

  public static final int OK = 0;
  public static final int ERROR = -1;
  public static final String SEP = ":";
  public static final String ESCAPE = "\\";

  private static final String READ = "0";
  private static final String INSERT = "2";
  private static final String UPDATE = "1";
  private static final String DELETE = "3";

  public static final String TRACE_FILE_PROPERTY = "kvtracer.tracefile";
  public static final String KEYMAP_FILE_PROPERTY = "kvtracer.keymapfile";

  private HashMap<String, HashSet<String>> mKeyMap = new HashMap<String, HashSet<String>>();
  private BufferedWriter mOpWriter;
  private BufferedWriter mKeyWriter;


  @Override
    public void init() throws DBException {
      mKeyMap.clear();
      Properties props = getProperties();
      String traceFileName = props.getProperty(TRACE_FILE_PROPERTY);
      String keymapFileName = props.getProperty(KEYMAP_FILE_PROPERTY);
      try {
        if (traceFileName == null || traceFileName.isEmpty()) {
          mOpWriter = new BufferedWriter(new OutputStreamWriter(System.out));
        } else {
          File traceFile = new File(traceFileName);
          if (!traceFile.exists()) {
            traceFile.createNewFile();
          }
          mOpWriter = new BufferedWriter(new FileWriter(traceFile));
        }
        if (keymapFileName != null && !keymapFileName.isEmpty()) {
          File keymapFile = new File(keymapFileName);
          if (keymapFile.exists()) {
            BufferedReader reader = new BufferedReader(new FileReader(keymapFile));
            String keyline = null;
            while ((keyline = reader.readLine()) != null) {
              String[] parts = decodeKey(keyline);  
              if (parts.length != 1) {
                throw new DBException("invalid keyline " + keyline + " in keymap file");
              }
              String prefix = encodeKey(parts[0], parts[0], "");
              HashSet<String> keys = mKeyMap.get(prefix);
              if (keys == null) {
                keys = new HashSet<String>();
                mKeyMap.put(prefix, keys);
              }
              keys.add(keyline);
            }
            reader.close();
          } else {
            keymapFile.createNewFile();
          }
          mKeyWriter = new BufferedWriter(new FileWriter(keymapFile));
        } else {
          throw new DBException("kvtracer.keymapfile is not set");
        }
      } catch (IOException exception) {
        throw new DBException(exception);
      }
    }

  @Override
    public void cleanup() throws DBException {
      try {
        if (mOpWriter != null) {
          mOpWriter.close();
        }
        if (mKeyWriter != null) {
          for (Map.Entry<String, HashSet<String>> entry : mKeyMap.entrySet()) {
            HashSet<String> keys = entry.getValue();
            for (String key : keys) {
              mKeyWriter.write(key + "\n");
            }
          }
          mKeyWriter.close();
        }
      } catch (IOException exception) {
        throw new DBException(exception);
      }
      mKeyMap.clear();
    }

  /**
   * Create a key from table name, key and field by escaping and concatenate them.
   */
  public static String encodeKey(String table, String key, String field) {
    return escape(key); 
  }

  /**
   * Decode an key that's encoded from encodeKey
   */
  public static String[] decodeKey(String encKey) {
    ArrayList<String> parts = new ArrayList<String>();
    int pos = -1;
    int begin = 0;
    int last = 0;
    int inc = SEP.length();
    int esclen = ESCAPE.length();
    int escesclen = 2 * esclen;
    boolean escape;
    while (last < encKey.length() && (pos = encKey.indexOf(SEP, last)) >= 0) {
      // System.out.println("pos=" + pos + ",begin=" + begin + ",last=" + last);
      escape = false;
      if (pos >= esclen) {
        if (encKey.substring(pos - esclen, pos).equals(ESCAPE)) {
          // if the separator is a \:
          if (pos >= escesclen) {
            if (!encKey.substring(pos - escesclen, pos - esclen).equals(ESCAPE)) {
              //  and not \\:, then this is an escaped separator, not real separator
              escape = true;
            }
          } else {
            escape = true;
          }
        }
        // this is an escaped separator, skip
      }
      if (!escape) {
        String part = encKey.substring(begin, pos);
        parts.add(unescape(part));
        begin = pos + inc;
      }
      last = pos + inc;
    }
    if (begin < encKey.length()) {
      String part = encKey.substring(begin);
      parts.add(unescape(part));
    }
    return parts.toArray(new String[parts.size()]);
  }

  public static String escape(String str) {
    String s1 = str.replace(ESCAPE, ESCAPE + ESCAPE);
    return s1.replace(SEP, ESCAPE + SEP);
  }

  public static String unescape(String encStr) {
    String s1 = encStr.replace(ESCAPE + SEP, SEP);
    return s1.replace(ESCAPE + ESCAPE, ESCAPE);
  }

  public static String removePrefix(String s, String prefix)
    {
        if (s != null && prefix != null && s.startsWith(prefix)){
            return s.substring(prefix.length());
        }
        return s;
    }

  @Override
    public Status read(String table, String key, Set<String> fields, Map<String, ByteIterator> result) {
      try {
        if (fields != null) {
          //for (String field:fields) {
          //  String encKey = encodeKey(table, removePrefix(key, "user"), field);
            mOpWriter.write(READ + " " +removePrefix(key, "user") + "\n");
         // }
        } else {
          mOpWriter.write(READ + " " + removePrefix(removePrefix(key, "user"), "user") + "\n");
          /*
          String prefix = encodeKey(table, removePrefix(key, "user"), "");
          HashSet<String> keys = mKeyMap.get(prefix);
          if (keys != null) {
            for (String k:keys) {
              mOpWriter.write(READ + " " + removePrefix(removePrefix(key, "user"), "user") + "\n");
            }
          }
        }*/
        }
        return Status.OK;
      } catch (IOException exception) {
        exception.printStackTrace();
        return Status.ERROR;
      }
    }

  @Override
    public Status scan(String table, String startkey, int recordcount, Set<String> fields, Vector<HashMap<String, ByteIterator>> result) {
      System.err.println("scan operation is not supported");
      return Status.ERROR;
    }

  @Override
   public Status update(String table, String key, Map<String, ByteIterator> values) {
      try {
        /*for (Map.Entry<String, ByteIterator> entry : values.entrySet()) {
          String prefix = encodeKey(table, removePrefix(key, "user"), "");
          String encKey = encodeKey(table, removePrefix(key, "user"), entry.getKey());
          HashSet<String> keys = mKeyMap.get(prefix);
          if (keys == null) {
            keys = new HashSet<String>();
            mKeyMap.put(prefix, keys);
          }
          keys.add(encKey);
          String value = entry.getValue().toString();
        
        }*/
        mOpWriter.write(UPDATE + " " + removePrefix(key, "user") + "\n");
        return Status.OK;
      } catch (IOException exception) {
        exception.printStackTrace();
        return Status.ERROR;
      }
    }

  @Override
    public Status insert(String table, String key, Map<String, ByteIterator> values) {
      try {
         mOpWriter.write(INSERT + " " + removePrefix(key, "user") + "\n");
        /*
        for (Map.Entry<String, ByteIterator> entry : values.entrySet()) {
          String prefix = encodeKey(table,removePrefix(key, "user"), "");
          String encKey = encodeKey(table, removePrefix(key, "user"), entry.getKey());
          HashSet<String> keys = mKeyMap.get(prefix);
          if (keys == null) {
            keys = new HashSet<String>();
            mKeyMap.put(prefix, keys);
          }
          keys.add(encKey);
          String value = entry.getValue().toString();
          mOpWriter.write(removePrefix(key, "user") + " " + INSERT + "\n");
        }*/
        return Status.OK;
      } catch (IOException exception) {
        exception.printStackTrace();
        return Status.ERROR;
      }
    }

  @Override
    public Status delete(String table, String key) {
      String prefix = encodeKey(table, removePrefix(key, "user"), "");
      HashSet<String> keys = mKeyMap.get(prefix);
      if (keys == null) {
        System.err.println("no keys to delete for <" + table + "," + key + ">");
        return Status.ERROR;
      }
      try {
        mOpWriter.write(DELETE + " " + removePrefix(key, "user")  +"\n");
        /*for (String k:keys) {
          String encKey = encodeKey(table, removePrefix(key, "user"), k);
          mOpWriter.write(removePrefix(key, "user") + " " + DELETE +"\n");
        }*/
      } catch (IOException exception) {
        exception.printStackTrace();
        return Status.ERROR;
      }
      return Status.OK;
    }
}
