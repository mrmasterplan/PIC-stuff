Attribute VB_Name = "Module1"
Option Explicit

   Public Const FILE_ATTRIBUTE_NORMAL = &H80
   Public Const FILE_FLAG_NO_BUFFERING = &H20000000
   Public Const FILE_FLAG_WRITE_THROUGH = &H80000000
   Public Const FILE_FLAG_OVERLAPPED = &H40000000
   Public Const ERROR_OPERATION_ABORTED = 995&
   Public Const ERROR_IO_INCOMPLETE = 996&
   Public Const ERROR_IO_PENDING = 997&

   Public Const PIPE_ACCESS_DUPLEX = &H3
   Public Const PIPE_READMODE_MESSAGE = &H2
   Public Const PIPE_TYPE_MESSAGE = &H4
   Public Const PIPE_WAIT = &H0

   Public Const FILE_BEGIN = 0
   Public Const FILE_CURRENT = 1
   Public Const FILE_END = 2

   Public Const INVALID_HANDLE_VALUE = -1

   Public Const SECURITY_DESCRIPTOR_MIN_LENGTH = (20)
   Public Const SECURITY_DESCRIPTOR_REVISION = (1)
   
   Public Const INFINITE = -1&

   Private Const QS_KEY = &H1&
   Private Const QS_MOUSEMOVE = &H2&
   Private Const QS_MOUSEBUTTON = &H4&
   Private Const QS_POSTMESSAGE = &H8&
   Private Const QS_TIMER = &H10&
   Private Const QS_PAINT = &H20&
   Private Const QS_SENDMESSAGE = &H40&
   Private Const QS_HOTKEY = &H80&
   Private Const QS_ALLINPUT = (QS_SENDMESSAGE Or QS_PAINT _
            Or QS_TIMER Or QS_POSTMESSAGE Or QS_MOUSEBUTTON _
            Or QS_MOUSEMOVE Or QS_HOTKEY Or QS_KEY)
        
   Type SECURITY_ATTRIBUTES
           nLength As Long
           lpSecurityDescriptor As Long
           bInheritHandle As Long
   End Type

   Private Type OVERLAPPED
      Internal As Long
      InternalHigh As Long
      offset As Long
      OffsetHigh As Long
      hEvent As Long
   End Type

   Public Const GMEM_FIXED = &H0
   Public Const GMEM_ZEROINIT = &H40
   Public Const GPTR = (GMEM_FIXED Or GMEM_ZEROINIT)

   Declare Function GlobalAlloc Lib "kernel32" ( _
      ByVal wFlags As Long, ByVal dwBytes As Long) As Long
   Declare Function GlobalFree Lib "kernel32" (ByVal hMem As Long) As Long
   Declare Function CreateNamedPipe Lib "kernel32" Alias _
      "CreateNamedPipeA" ( _
      ByVal lpName As String, _
      ByVal dwOpenMode As Long, _
      ByVal dwPipeMode As Long, _
      ByVal nMaxInstances As Long, _
      ByVal nOutBufferSize As Long, _
      ByVal nInBufferSize As Long, _
      ByVal nDefaultTimeOut As Long, _
      lpSecurityAttributes As Any) As Long

   Declare Function InitializeSecurityDescriptor Lib "advapi32.dll" ( _
      ByVal pSecurityDescriptor As Long, _
      ByVal dwRevision As Long) As Long

   Declare Function SetSecurityDescriptorDacl Lib "advapi32.dll" ( _
      ByVal pSecurityDescriptor As Long, _
      ByVal bDaclPresent As Long, _
      ByVal pDacl As Long, _
      ByVal bDaclDefaulted As Long) As Long

   Declare Function ConnectNamedPipe Lib "kernel32" ( _
      ByVal hNamedPipe As Long, _
      lpOverlapped As Any) As Long

   Declare Function DisconnectNamedPipe Lib "kernel32" ( _
      ByVal hNamedPipe As Long) As Long

   Declare Function WriteFile Lib "kernel32" ( _
      ByVal hFile As Long, _
      lpBuffer As Any, _
      ByVal nNumberOfBytesToWrite As Long, _
      lpNumberOfBytesWritten As Long, _
      lpOverlapped As Any) As Long

   Declare Function ReadFile Lib "kernel32" ( _
      ByVal hFile As Long, _
      lpBuffer As Any, _
      ByVal nNumberOfBytesToRead As Long, _
      lpNumberOfBytesRead As Long, _
      lpOverlapped As Any) As Long

   Declare Function FlushFileBuffers Lib "kernel32" ( _
      ByVal hFile As Long) As Long

   Declare Function CloseHandle Lib "kernel32" ( _
      ByVal hObject As Long) As Long

   Declare Function SetFilePointer Lib "kernel32" ( _
      ByVal hFile As Long, _
      ByVal lDistanceToMove As Long, _
      lpDistanceToMoveHigh As Long, _
      ByVal dwMoveMethod As Long) As Long

   Declare Function CreateThread Lib "kernel32" ( _
      ByVal lpSecurityAttributes As Long, _
      ByVal dwStackSize As Long, _
      ByVal lpStartAddress As Long, _
      ByVal lpParameter As Long, _
      ByVal dwCreationFlags As Long, _
      lpThreadId As Long) As Long

    Declare Function CreateEvent Lib "kernel32" Alias "CreateEventA" ( _
      lpEventAttributes As Long, _
      ByVal bManualReset As Long, _
      ByVal bInitialState As Long, _
      ByVal lpName As String) As Long

    Declare Function WaitForMultipleObjects Lib "kernel32" ( _
        ByVal nCount As Long, _
        lpHandles As Long, _
        ByVal bWaitAll As Long, _
        ByVal dwMilliseconds As Long) As Long

    Declare Function MsgWaitForMultipleObjects Lib "user32" ( _
        ByVal nCount As Long, _
        pHandles As Long, _
        ByVal fWaitAll As Long, _
        ByVal dwMilliseconds As Long, _
        ByVal dwWakeMask As Long) As Long
        
    Private pSD As Long
    Private sa As SECURITY_ATTRIBUTES
    Private hPipe As Long
    Private Const szPipeName = "\\.\pipe\rrpipe"
    ' size of the pipe buffer used to communicate to/from RR
    Private Const PIPE_BUFFER_SIZE = 4096
    ' buffer size used when reading in variable data
    Private Const DATA_BUFFER = 1024
    ' the maximum length of a variable name to read
    Private Const MAX_VARNAME_SIZE = 64
    
    ' hold the name used in RR to identify this module
    Private imageName As String
    ' image processed count ... sent back to RR
    Private imageCount As Long
    ' dimensions of the received image
    Private imageWidth As Integer, imageHeight As Integer
    ' holds the image data
    Private imagePixels() As Byte
    Public Running As Boolean
    Public gbl_hStopEvent As Long
    
    Public variables As Dictionary
       
    Public Sub Main()
              
      Dim i As Long, dwOpenMode As Long, dwPipeMode As Long
      Dim res As Long, bytesRead As Long, varLen As Long
      Dim varName() As Byte
      Dim varData(1024) As Byte
          
      Running = True
      
      Set variables = New Dictionary
            
      'Create the NULL security token for the pipe
      pSD = GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH)
      res = InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)
      res = SetSecurityDescriptorDacl(pSD, -1, 0, 0)
      sa.nLength = LenB(sa)
      sa.lpSecurityDescriptor = pSD
      sa.bInheritHandle = True
      
      Dim op As OVERLAPPED

      Dim eventHandle As Long
      eventHandle = CreateEvent(ByVal 0&, False, False, ByVal 0&)
      op.hEvent = eventHandle
      
      'Create the Named Pipe
      dwOpenMode = PIPE_ACCESS_DUPLEX Or FILE_FLAG_WRITE_THROUGH
      'dwPipeMode = FILE_FLAG_OVERLAPPED Or PIPE_TYPE_MESSAGE Or PIPE_READMODE_MESSAGE
      dwPipeMode = PIPE_TYPE_MESSAGE Or PIPE_READMODE_MESSAGE
      hPipe = CreateNamedPipe(szPipeName, dwOpenMode, dwPipeMode, 10, 10000, 2000, 10000, sa)

      Do While Running
         
          'Waiting
          
          res = ConnectNamedPipe(hPipe, ByVal 0)
          
          'res = MsgWaitForMultipleObjects(1, eventHandle, 0&, INFINITE, QS_ALLINPUT)

          If res > 0 Then
          
              'Connected
              
              imageCount = 0
              
              varLen = 0
        
              Do While Running
                 
                 imageCount = imageCount + 1
                 
                 'Processing  & imageCount
                 
                 Do While Running
                    
                    DoEvents
                                        
                    ' read in variable length
                    bytesRead = 4
                    ReadFile hPipe, varLen, 4, bytesRead, ByVal 0
                    ' if length <=0 on the variable name then we're done
                    If varLen <= 0 Then Exit Do
                    ' read in variable name but if the name is longer than 64 characters
                    ' then grab the first 64 chars only
                    If varLen < MAX_VARNAME_SIZE Then
                        bytesRead = varLen
                        ReDim varName(varLen - 1)
                        ReadFile hPipe, varName(0), varLen, bytesRead, ByVal 0
                    Else
                        bytesRead = 64
                        ' read in first 64 chars
                        ReDim varName(64 - 1)
                        ReadFile hPipe, varName(0), MAX_VARNAME_SIZE - 1, bytesRead, ByVal 0
                        ' skip over remaining chars
                        SetFilePointer hPipe, varLen - MAX_VARNAME_SIZE + 1, 0, FILE_CURRENT
                    End If
                
                    ' read in the variable's data length
                    bytesRead = 4
                    ReadFile hPipe, varLen, 4, bytesRead, ByVal 0
                    ' if the data is less than 1024 read it in now ..
                    If varLen < 1024 Then
                        bytesRead = varLen
                        ReadFile hPipe, varData(0), varLen, bytesRead, ByVal 0
                    End If
                 
                    ' handle this variable
                    ProcessVariable hPipe, varName, varData, varLen
                    
                 Loop
                 
                 ' termination signal -1 on attribute length
                 If varLen < 0 Then Exit Do
                 
                 ' process image
                Process
        
                ' Write out the processed image back to RoboRealm using stdout.
                ' You can also write back any other variables to use in
                ' other parts of the program.
                ' The format is the same as the input.
                ReturnBytesVariable hPipe, CStr("image"), imagePixels, CLng(imageWidth) * CLng(imageHeight) * 3
        
                ' Send back the count as an example of how to feed back variables into RoboRealm
                ReturnIntVariable hPipe, "count", imageCount
        
                ' write out end of message
                varLen = 0
                Dim bytesWrite
                WriteFile hPipe, varLen, 4, bytesWrite, ByVal 0
                FlushFileBuffers hPipe
              
                If bytesWrite <= 0 Then Exit Do
                
                ' continue by waiting for next image request
            Loop
            
            'Disconnected
        
            ' signal end of stream
            Dim eos
            eos = -1
            WriteFile hPipe, varLen, 4, bytesWrite, ByVal 0
            FlushFileBuffers hPipe
        
            DisconnectNamedPipe hPipe
          
            If varLen < 0 Then Exit Do
            
         End If
    Loop
    
    'Exiting"
    
    'Close the pipe handle
    CloseHandle hPipe
    GlobalFree pSD
    
   End Sub

   ' returns an error message to RR. This message is displayed in the "messages" list within
   ' the RR Pipe Program interface.
    Private Sub ReturnError(ByVal hPipe As Long, ByRef txt As String)

        Dim bytesWrite As Long
        Dim writeLen As Integer
        Dim txtBytes() As Byte
        Dim dataLen As Integer
        
        writeLen = 5
        ' write the message name
        WriteFile hPipe, writeLen, 4, bytesWrite, ByVal 0
        WriteFile hPipe, "error", writeLen, bytesWrite, ByVal 0
        ' write the message data
        dataLen = Len(txt)
        txtBytes = StrConv(txt, vbFromUnicode)
        WriteFile hPipe, dataLen, 4, bytesWrite, ByVal 0
        WriteFile hPipe, txtBytes(0), dataLen, bytesWrite, ByVal 0
        
    End Sub

    ' returns a byte string variable to RR. The returned variables can be used
    ' elsewhere in RR for continued processing.
    Private Sub ReturnBytesVariable(ByVal hPipe As Long, ByRef name As String, ByRef data() As Byte, ByVal dataLen As Long)

        Dim bytesWrite As Long
        Dim writeLen As Integer
        writeLen = Len(name)
        Dim varData() As Byte
        
        varData = StrConv(name, vbFromUnicode)
        
        ' write the message name
        WriteFile hPipe, writeLen, 4, bytesWrite, ByVal 0
        WriteFile hPipe, varData(0), writeLen, bytesWrite, ByVal 0
        ' write the message data
        WriteFile hPipe, dataLen, 4, bytesWrite, ByVal 0
        If dataLen > 0 Then
            WriteFile hPipe, data(0), dataLen, bytesWrite, ByVal 0
        End If
        
    End Sub

    Private Sub ReturnIntVariable(ByVal hPipe As Long, ByRef name As String, ByVal data As Long)

        Dim bytesWrite As Long
        Dim dataLen As Integer
        Dim dataBytes() As Byte
        Dim dataText As String
        
        Dim varData() As Byte
        
        varData = StrConv(name, vbFromUnicode)
        
        dataText = CStr(data)
        dataBytes = StrConv(dataText, vbFromUnicode)
        
        dataLen = Len(name)
        ' write the message name
        WriteFile hPipe, dataLen, 4, bytesWrite, ByVal 0
        WriteFile hPipe, varData(0), dataLen, bytesWrite, ByVal 0
        ' write the message data
        dataLen = Len(dataText)
        WriteFile hPipe, dataLen, 4, bytesWrite, ByVal 0
        WriteFile hPipe, dataBytes(0), dataLen, bytesWrite, ByVal 0
        
    End Sub

    ' Parses the variables sent by RR into the appropriate structure. You can add
    ' your own processing routines here to handle other variables that may get sent.
    Private Sub ProcessVariable(ByVal hPipe As Long, ByRef name() As Byte, ByRef data() As Byte, ByVal dataLen As Long)

        Dim strName As String
        
        strName = StrConv(name, vbUnicode)

        ' determine what we've got
        If strName = "name" Then
            imageName = StrConv(data, vbUnicode)
            imageName = Left(imageName, dataLen)
        ' determine what we've got
        ElseIf strName = "width" Then
            imageWidth = data(0) + (data(1) * 256)
        ElseIf strName = "height" Then
            imageHeight = data(0) + (data(1) * 256)
        ElseIf strName = "image" Then
            If imageWidth = 0 Or imageHeight = 0 Then
                ReturnError hPipe, "Error - missing image dimensions before image data!"
                End
            End If
    
            If dataLen <> CLng(imageWidth) * CLng(imageHeight) * 3 Then
                ReturnError hPipe, "Error - length of data and dimensions of image" & vbCrLf & "           disagree! (width:" & imageWidth & " height:" & imageHeight & " len:" & dataLen & ")" & vbCrLf
                End
            End If

            ReDim imagePixels(dataLen)
            
            ' we did not read in the image data yet since it is always > 1024 ..
            Dim bytesRead
            ReadFile hPipe, imagePixels(0), dataLen, bytesRead, ByVal 0
        ' skip this variable
        Else
          If dataLen > DATA_BUFFER Then
            ' only need to skip bytes if we have not read it in yet
            SetFilePointer hPipe, dataLen, 0, FILE_CURRENT
          Else
            Dim strData
            strData = StrConv(data, vbUnicode)
            strData = Left(strData, dataLen)
            variables.Item(strName) = strData
          End If
        End If
        
    End Sub

    ' Performs the image conversion/processing/etc that you want to perform
    Sub Process()
        
        Dim i As Long
        Dim tmp As Byte
        Dim w As Long
        
        ' you can access any variables using
        ' variables.item("name")
        ' MsgBox variables.Item("COG_X")
        ' msgbox variables.item("COG_Y")
        
        ' remember that we're working in RGB color ... so the actual width is 3x
        w = imageWidth * 3

        ' sanity check
        If imageWidth = 0 Or imageHeight = 0 Then Exit Sub
        
        ' you could check imageName to perform different analysis in the same
        ' program ....

        ' as an example ... swap red and blue values
        For i = 0 To ((w * imageHeight) - 3) Step 3
            ' image format is RGB ... first byte is red
            tmp = imagePixels(i)
            ' swap red and blue
            imagePixels(i) = imagePixels(i + 2)
            imagePixels(i + 2) = tmp
        Next
        
        ' note this this is an inplace filter so you don't need an additional image array
        ' but you've now corrupted the original image ...

    End Sub
 
