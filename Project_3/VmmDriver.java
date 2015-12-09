import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;

public class VmmDriver {

    private final int TABLE_ENTRIES_OR_SIZE = 256;
    private final int PHYSICAL_MEMORY_SCALAR = 1;
    private int [] pageTable = new int [TABLE_ENTRIES_OR_SIZE];
    private int [] frameTable = new int [TABLE_ENTRIES_OR_SIZE]; 
    private HashMap victimFrameLocater = new HashMap<Integer,Integer>();  
    private ArrayList<Integer> framesFIFOQueue = new ArrayList<Integer>();
    private HashMap TLB = new HashMap<Integer,Integer>(17, 1);  
    private ArrayList<Integer> TLB_FIFOQueue = new ArrayList<Integer>();
    private byte [] physicalMemory = new byte [TABLE_ENTRIES_OR_SIZE*TABLE_ENTRIES_OR_SIZE];
    private int physicalMemoryPointer = 0;
    private int availableFrameCounter = 0;
    private int bitMask = 0x00FF;
    private double pageFaults, numOfAddresses, pageFaultRate, TLBHits = 0;

    public static void main(String[] args) {
        new VmmDriver().run();
    }        
    public void run(){

        //Input File with Virtual Addresses
        String filename = ("input.txt");

        // the file representing the simulated  disk
        File backStorePath;
        RandomAccessFile backingStore = null;

        /*Create pageTable and 
         * initialize each index with -1.
         * This indicates that the entry in
         * the pageTable is invalid.*/
        for(int i = 0; i <(TABLE_ENTRIES_OR_SIZE-1); i++){
            pageTable[i] = -1; }


        /*Create frameTable and 
         * initialize each index starting at 0
         * and increasing by TABLE_ENTRIES_OR_SIZE
         * at each iteration*/
        int frameNum = 0;
        for(int i = 0; i < (TABLE_ENTRIES_OR_SIZE)-1; i++){
            frameNum = i*(TABLE_ENTRIES_OR_SIZE);
            frameTable[i] = frameNum;}

        BufferedReader br = null;  
        try
        {   

            File file = new File(filename);   
            FileReader fr = new FileReader(file);
            br = new BufferedReader(fr);
            String data;

            while((data = br.readLine()) != null)    
            {   
                /*Right-Shift virtual Address by 8 bits virtual 
                 * address space256).
                 * Use bitMask to "mask" Least Significant 0xFF bits*/
                int pageNumber = (Integer.parseInt(data) >> 8);
                int offset = (Integer.parseInt(data) & bitMask);

                int val = 0;
                int frameNumber = 0;

                //Page Fault handling
                if((!(TLB.containsKey(pageNumber)) && pageTable[pageNumber] == -1)){

                    try {
                        backStorePath = new 
                                File("BACKING_STORE.bin");
                        backingStore = new RandomAccessFile(backStorePath, "r");


                        // seek to byte position pageNumber * TABLE_ENTRIES_OR_SIZE
                        backingStore.seek((pageNumber)*(TABLE_ENTRIES_OR_SIZE));
                        backingStore.read(physicalMemory, physicalMemoryPointer, TABLE_ENTRIES_OR_SIZE);

                        val = physicalMemory[physicalMemoryPointer+offset];

                        physicalMemoryPointer +=TABLE_ENTRIES_OR_SIZE;

                        //Update PageTable
                        pageTable[pageNumber] = frameTable[availableFrameCounter];
                        frameNumber = pageTable[pageNumber];

                        availableFrameCounter++;

                        //Update TLB
                        if(TLB.size() <= 16){
                            addtoTLB(pageNumber,frameNumber);
                        }
                        else{
                            TLB.remove(TLB_FIFOQueue.get(0));

                        }

                        pageFaults++;
                        //Place value in physicalMemory
                        //physicalMemory[frameNumber] = val;
                        //Take Frame number "or" it with offset to get physical address
                        int physicalAddress = frameNumber  | offset;
                        System.out.println("Virtual address: " + data +" Physical address: " 
                                + physicalAddress + " Value: " + val);
                        //System.out.println("PageNumber: " + pageNumber
                        //+ " FrameNumber: " + frameNumber + " Offset: " + offset);
                        numOfAddresses++;
                    }
                    catch (IOException e) {
                        System.err.println ("Unable to start the disk");
                        //System.exit(1);
                    }
                    finally {
                        backingStore.close();
                    }

                }//if
                else{
                    if(TLB.containsKey(pageNumber)){

                        frameNumber = checkTLB (pageNumber);
                        int physicalAddress = frameNumber | offset;
                        System.out.println("Virtual address: " + data +" Physical address: " 
                                + physicalAddress + " Value: " +  physicalMemory[frameNumber+offset]);
                        TLBHits++;
                        numOfAddresses++;
                    }
                    else{                //Take pageNumber as index and get Frame number from Page table
                        frameNumber = pageTable[pageNumber];
                        //Take Frame number  "or" it with offset to get physical address
                        int physicalAddress = frameNumber | offset;
                        System.out.println("Virtual address: " + data +" Physical address: " 
                                + physicalAddress + " Value: " +  physicalMemory[frameNumber+offset]);
                        //System.out.println("PageNumber: " + pageNumber + 
                        //" FrameNumber: " + frameNumber + " Offset: " + offset);
                        numOfAddresses++;
                    }            
                }//else
            }//while
        }//try
        catch(ArrayIndexOutOfBoundsException e){
            e.printStackTrace();
        }
        catch(IOException e){
            System.out.println(e + "File: " + "\"" + filename + "\"" + " not Found.");
        }
        finally{
            try{
                if(br != null){
                    br.close();
                }
            }
            catch(IOException e){
                e.printStackTrace();
            }
        }
        printStats();
    }//run()
    public void addtoTLB (int pageNumber, int frameNumber ){     
        // Put value into Hash Map
        TLB.put(pageNumber, frameNumber);
        TLB_FIFOQueue.add(pageNumber);

    }//addtoTLB
    public int checkTLB (int pageNumber){
        return (int) TLB.get(pageNumber);

    }//checkTLB

    public void printStats(){
        pageFaultRate = pageFaults/numOfAddresses;
        System.out.println("Number of Translated Addresses = " + (int)numOfAddresses);
        System.out.println("Page Faults = " + (int)pageFaults);
        System.out.println("Page Fault Rate = " + pageFaultRate);
        System.out.println("TLB Hits = " + (int)TLBHits);
        System.out.println("TLB Hit Rate = " + (TLBHits/numOfAddresses));
    }

}