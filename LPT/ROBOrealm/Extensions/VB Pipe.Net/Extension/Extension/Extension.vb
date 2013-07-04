Imports System.Text.Encoding

Module modMain

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

    Public Structure SECURITY_ATTRIBUTES
        Public nLength As Integer
        Public lpSecurityDescriptor As Integer
        Public bInheritHandle As Integer
    End Structure

    Public Structure OVERLAPPED
        Public Internal As Integer
        Public InternalHigh As Integer
        Public offset As Integer
        Public OffsetHigh As Integer
        Public hEvent As Integer
    End Structure

    Public Const GMEM_FIXED = &H0
    Public Const GMEM_ZEROINIT = &H40
    Public Const GPTR = (GMEM_FIXED Or GMEM_ZEROINIT)

    Declare Function GlobalAlloc Lib "kernel32" ( _
       ByVal wFlags As Integer, ByVal dwBytes As Integer) As Integer
    Declare Function GlobalFree Lib "kernel32" (ByVal hMem As Integer) As Integer
    Declare Function CreateNamedPipe Lib "kernel32" Alias _
       "CreateNamedPipeA" ( _
       ByVal lpName As String, _
       ByVal dwOpenMode As Integer, _
       ByVal dwPipeMode As Integer, _
       ByVal nMaxInstances As Integer, _
       ByVal nOutBufferSize As Integer, _
       ByVal nInBufferSize As Integer, _
       ByVal nDefaultTimeOut As Integer, _
    ByRef lpSecurityAttributes As SECURITY_ATTRIBUTES) As Integer

    Declare Function InitializeSecurityDescriptor Lib "advapi32.dll" ( _
       ByVal pSecurityDescriptor As Integer, _
       ByVal dwRevision As Integer) As Integer

    Declare Function SetSecurityDescriptorDacl Lib "advapi32.dll" ( _
       ByVal pSecurityDescriptor As Integer, _
       ByVal bDaclPresent As Integer, _
       ByVal pDacl As Integer, _
       ByVal bDaclDefaulted As Integer) As Integer

    Declare Function ConnectNamedPipe Lib "kernel32" ( _
        ByVal hNamedPipe As Integer, _
        ByRef lpOverlapped As OVERLAPPED) As Integer

    Declare Function DisconnectNamedPipe Lib "kernel32" ( _
        ByVal hNamedPipe As Integer) As Integer

    Declare Function WriteFile Lib "kernel32" ( _
        ByVal hFile As Integer, _
        ByVal lpBuffer As Byte(), _
        ByVal nNumberOfBytesToWrite As Integer, _
        ByRef lpNumberOfBytesWritten As Integer, _
        ByRef lpOverlapped As OVERLAPPED) As Integer

    Declare Function ReadFile Lib "kernel32" ( _
        ByVal hFile As Integer, _
        ByRef lpBuffer As Byte, _
        ByVal nNumberOfBytesToRead As Integer, _
        ByRef lpNumberOfBytesRead As Integer, _
        ByRef lpOverlapped As OVERLAPPED) As Integer

    Declare Function FlushFileBuffers Lib "kernel32" ( _
        ByVal hFile As Integer) As Integer

    Declare Function CloseHandle Lib "kernel32" ( _
        ByVal hObject As Integer) As Integer

    Declare Function SetFilePointer Lib "kernel32" ( _
        ByVal hFile As Integer, _
        ByVal lDistanceToMove As Integer, _
        ByVal lpDistanceToMoveHigh As Integer, _
        ByVal dwMoveMethod As Integer) As Integer

    Declare Function CreateThread Lib "kernel32" ( _
       ByVal lpSecurityAttributes As Integer, _
       ByVal dwStackSize As Integer, _
       ByVal lpStartAddress As Integer, _
       ByVal lpParameter As Integer, _
       ByVal dwCreationFlags As Integer, _
    ByVal lpThreadId As Integer) As Integer

    Declare Function CreateEvent Lib "kernel32" Alias "CreateEventA" ( _
        ByVal lpEventAttributes As Integer, _
        ByVal bManualReset As Integer, _
        ByVal bInitialState As Integer, _
        ByVal lpName As String) As Integer

    Declare Function WaitForMultipleObjects Lib "kernel32" ( _
        ByVal nCount As Integer, _
        ByVal lpHandles As Integer, _
        ByVal bWaitAll As Integer, _
        ByVal dwMilliseconds As Integer) As Integer

    Declare Function MsgWaitForMultipleObjects Lib "user32" ( _
        ByVal nCount As Integer, _
        ByVal pHandles As Integer, _
        ByVal fWaitAll As Integer, _
        ByVal dwMilliseconds As Integer, _
        ByVal dwWakeMask As Integer) As Integer

    Private pSD As Integer
    Private sa As SECURITY_ATTRIBUTES
    Private hPipe As Integer
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
    Private imageCount As Integer
    ' dimensions of the received image
    Private imageWidth As Integer, imageHeight As Integer
    ' holds the image data
    Private imagePixels() As Byte
    Public Running As Boolean
    Public gbl_hStopEvent As Integer

    Dim variables As New Hashtable

    Private Function intToByte(ByVal i As Integer)

        Dim bArray As Byte() = {i And 255, (i >> 8) And 255, (i >> 16) And 255, (i >> 24) And 255}
        intToByte = bArray

    End Function


    Sub Main()

        Dim dwOpenMode As Integer, dwPipeMode As Integer
        Dim res As Integer, bytesRead As Integer, varLen As Integer
        Dim varName() As Byte
        Dim varData(1024) As Byte

        Running = True

        'Create the NULL security token for the pipe
        pSD = GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH)
        res = InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)
        res = SetSecurityDescriptorDacl(pSD, -1, 0, 0)
        sa.nLength = Len(sa)
        sa.lpSecurityDescriptor = pSD
        sa.bInheritHandle = True

        Dim op As OVERLAPPED

        Dim eventHandle As Long
        eventHandle = CreateEvent(0&, False, False, 0&)
        op.hEvent = eventHandle

        'Create the Named Pipe
        dwOpenMode = PIPE_ACCESS_DUPLEX Or FILE_FLAG_WRITE_THROUGH
        'dwPipeMode = FILE_FLAG_OVERLAPPED Or PIPE_TYPE_MESSAGE Or PIPE_READMODE_MESSAGE
        dwPipeMode = PIPE_TYPE_MESSAGE Or PIPE_READMODE_MESSAGE
        hPipe = CreateNamedPipe(szPipeName, dwOpenMode, dwPipeMode, 10, 10000, 2000, 10000, sa)

        Do While Running

            'Waiting

            res = ConnectNamedPipe(hPipe, Nothing)

            'res = MsgWaitForMultipleObjects(1, eventHandle, 0&, INFINITE, QS_ALLINPUT)

            If res > 0 Then

                'Connected

                imageCount = 0

                Dim bytesWrite = 0

                varLen = 0

                Do While Running

                    imageCount = imageCount + 1

                    'Processing  & imageCount

                    Do While Running

                        'DoEvents()

                        ' read in variable length
                        varLen = ReadInteger(hPipe)
                        ' if length <=0 on the variable name then we're done
                        If varLen <= 0 Then Exit Do
                        ' read in variable name but if the name is longer than 64 characters
                        ' then grab the first 64 chars only
                        If varLen < MAX_VARNAME_SIZE Then
                            bytesRead = varLen
                            ReDim varName(varLen - 1)
                            ReadFile(hPipe, varName(0), varLen, bytesRead, Nothing)
                        Else
                            bytesRead = 64
                            ' read in first 64 chars
                            ReDim varName(64 - 1)
                            ReadFile(hPipe, varName(0), MAX_VARNAME_SIZE - 1, bytesRead, Nothing)
                            ' skip over remaining chars
                            SetFilePointer(hPipe, varLen - MAX_VARNAME_SIZE + 1, 0, FILE_CURRENT)
                        End If

                        ' read in the variable's data length
                        varLen = ReadInteger(hPipe)
                        ' if the data is less than 1024 read it in now ..
                        If varLen < 1024 Then
                            bytesRead = varLen
                            ReadFile(hPipe, varData(0), varLen, bytesRead, Nothing)
                        End If

                        ' handle this variable
                        ProcessVariable(hPipe, varName, varData, varLen)

                    Loop

                    ' termination signal -1 on attribute length
                    If varLen < 0 Then Exit Do

                    ' process image
                    Process()

                    ' Write out the processed image back to RoboRealm using stdout.
                    ' You can also write back any other variables to use in
                    ' other parts of the program.
                    ' The format is the same as the input.
                    ReturnBytesVariable(hPipe, CStr("image"), imagePixels, CLng(imageWidth) * CLng(imageHeight) * 3)

                    ' Send back the count as an example of how to feed back variables into RoboRealm
                    ReturnIntVariable(hPipe, "count", imageCount)

                    ' write out end of message
                    WriteFile(hPipe, ASCII.GetBytes(""), 4, bytesWrite, Nothing)
                    FlushFileBuffers(hPipe)

                    If bytesWrite <= 0 Then Exit Do

                    ' continue by waiting for next image request
                Loop

                'Disconnected

                ' signal end of stream
                Dim eos
                eos = -1
                WriteFile(hPipe, intToByte(varLen), 4, bytesWrite, Nothing)
                FlushFileBuffers(hPipe)

                DisconnectNamedPipe(hPipe)

                If varLen < 0 Then Exit Do

            End If
        Loop

        'Exiting"

        'Close the pipe handle
        CloseHandle(hPipe)
        GlobalFree(pSD)

    End Sub

    ' read in an integer from the pipe. Since we read in bytes we need to change those bytes to
    ' their integer counterparts.
    Private Function ReadInteger(ByRef hPipe)

        Dim bytesRead As Integer
        Dim data As Byte() = {0, 0, 0, 0}
        bytesRead = 4

        ReadFile(hPipe, data(0), 4, bytesRead, Nothing)

        ReadInteger = data(0) Or (data(1) * 256) Or (data(2) * 65536) Or (data(3) * 16777216)

    End Function


    ' returns an error message to RR. This message is displayed in the "messages" list within
    ' the RR Pipe Program interface.
    Private Sub ReturnError(ByVal hPipe As Long, ByRef txt As String)

        Dim bytesWrite As Long
        Dim writeLen As Integer
        Dim txtBytes As Byte()
        Dim dataLen As Integer

        writeLen = 5
        ' write the message name
        WriteFile(hPipe, intToByte(writeLen), 4, bytesWrite, Nothing)
        WriteFile(hPipe, ASCII.GetBytes("error"), writeLen, bytesWrite, Nothing)
        ' write the message data
        dataLen = Len(txt)
        txtBytes = ASCII.GetBytes(txt)
        WriteFile(hPipe, intToByte(dataLen), 4, bytesWrite, Nothing)
        WriteFile(hPipe, txtBytes, dataLen, bytesWrite, Nothing)

    End Sub

    ' returns a byte string variable to RR. The returned variables can be used
    ' elsewhere in RR for continued processing.
    Private Sub ReturnBytesVariable(ByVal hPipe As Long, ByRef name As String, ByRef data As Byte(), ByVal dataLen As Long)

        Dim bytesWrite As Long
        Dim writeLen As Integer
        writeLen = Len(name)
        Dim varData As Byte()

        varData = ASCII.GetBytes(name)

        ' write the message name
        WriteFile(hPipe, intToByte(writeLen), 4, bytesWrite, Nothing)
        WriteFile(hPipe, varData, writeLen, bytesWrite, Nothing)
        ' write the message data
        WriteFile(hPipe, intToByte(dataLen), 4, bytesWrite, Nothing)
        If dataLen > 0 Then
            WriteFile(hPipe, data, dataLen, bytesWrite, Nothing)
        End If

    End Sub

    Private Sub ReturnIntVariable(ByVal hPipe As Long, ByRef name As String, ByVal data As Long)

        Dim bytesWrite As Long
        Dim dataLen As Integer
        Dim dataBytes As Byte()
        Dim dataText As String

        Dim varData As Byte()

        varData = ASCII.GetBytes(name)

        dataText = CStr(data)
        dataBytes = ASCII.GetBytes(dataText)

        dataLen = Len(name)
        ' write the message name
        WriteFile(hPipe, intToByte(dataLen), 4, bytesWrite, Nothing)
        WriteFile(hPipe, varData, dataLen, bytesWrite, Nothing)
        ' write the message data
        dataLen = Len(dataText)
        WriteFile(hPipe, intToByte(dataLen), 4, bytesWrite, Nothing)
        WriteFile(hPipe, dataBytes, dataLen, bytesWrite, Nothing)

    End Sub

    ' Parses the variables sent by RR into the appropriate structure. You can add
    ' your own processing routines here to handle other variables that may get sent.
    Private Sub ProcessVariable(ByVal hPipe As Long, ByRef name As Byte(), ByRef data As Byte(), ByVal dataLen As Long)

        Dim strName As String

        strName = ASCII.GetString(name)

        ' determine what we've got
        If strName = "name" Then
            imageName = ASCII.GetString(data)
            imageName = imageName.Substring(0, dataLen)
            ' determine what we've got
        ElseIf strName = "width" Then
            imageWidth = data(0) + (data(1) * 256)
        ElseIf strName = "height" Then
            imageHeight = data(0) + (data(1) * 256)
        ElseIf strName = "image" Then
            If imageWidth = 0 Or imageHeight = 0 Then
                ReturnError(hPipe, "Error - missing image dimensions before image data!")
                End
            End If

            If dataLen <> CLng(imageWidth) * CLng(imageHeight) * 3 Then
                ReturnError(hPipe, "Error - length of data and dimensions of image" & vbCrLf & "           disagree! (width:" & imageWidth & " height:" & imageHeight & " len:" & dataLen & ")" & vbCrLf)
                End
            End If

            ReDim imagePixels(dataLen)

            ' we did not read in the image data yet since it is always > 1024 ..
            Dim bytesRead = 0
            ReadFile(hPipe, imagePixels(0), dataLen, bytesRead, Nothing)
            ' skip this variable
        Else
            If dataLen > DATA_BUFFER Then
                ' only need to skip bytes if we have not read it in yet
                SetFilePointer(hPipe, dataLen, 0, FILE_CURRENT)
            Else
                Dim strData As String
                strData = ASCII.GetString(data)
                strData = strData.Substring(0, dataLen)
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


End Module

