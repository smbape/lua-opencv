Option Explicit ' Force explicit variable declaration.

Private Function UpdateSeries(oSeriesCollection As SeriesCollection, Index As Integer, sSerieName As String, Values As Range)
    Dim oSeries As Series
    Dim oTrendline As Trendline
    Dim oTrendLines As Trendlines

    If oSeriesCollection.Count < Index Then
        Set oSeries = oSeriesCollection.NewSeries()
    Else
        Set oSeries = oSeriesCollection(Index)
    End If

    With oSeries
        .Name = sSerieName
        .Values = Values

        If .Trendlines.Count = 0 Then
            Set oTrendline = .Trendlines.Add(Type:=xlLinear)
        Else
            Set oTrendline = .Trendlines(1)
        End If

        With oTrendline
            .Type = xlLinear
            .DisplayRSquared = False
            .DisplayEquation = False
        End With

        oTrendline.Delete
    End With
End Function

Private Function FindColdId(oWorksheet As Worksheet, sColName As String) As String
    Dim iColIndex As Integer
    Dim sColId As String
    Dim sValue As String

    iColIndex = 0
    Do While True
        sColId = Chr(Asc("A") + iColIndex)
        sValue = oWorksheet.Range(sColId & "1").Value
        If sValue = "" Or sValue = sColName Then
            Exit Do
        End If
        iColIndex = iColIndex + 1
    Loop

    If oWorksheet.Range(sColId & "1").Value <> sColName Then
        FindColdId = ""
    Else
        FindColdId = sColId
    End If
End Function

Private Function AddSeriesByColName(oCompareWorksheet As Worksheet, oSeriesCollection As SeriesCollection, Index As Integer, oWorksheet As Worksheet, sColName As String, sSerieName As String)
    Dim iSerieRow As Integer
    Dim iRowIndex As Integer
    Dim sColId As String

    sColId = FindColdId(oWorksheet, sColName)

    If sColId = "" Then
        Exit Function
    End If

    oWorksheet.Columns(sColId).AutoFit

    iSerieRow = 3
    Do While oCompareWorksheet.Range("A" & iSerieRow).Value <> "" And oCompareWorksheet.Range("A" & iSerieRow).Value <> sSerieName
        iSerieRow = iSerieRow + 1
    Loop

    If oCompareWorksheet.Range("A" & iSerieRow).Value = "" Then
        oCompareWorksheet.Range("A" & iSerieRow).Value = sSerieName
    End If

    iRowIndex = 2
    Do While oWorksheet.Range(sColId & iRowIndex).Value <> ""
        iRowIndex = iRowIndex + 1
    Loop

    If iRowIndex > 2 Then
        iRowIndex = iRowIndex - 1
        UpdateSeries oSeriesCollection, Index, "='" & oCompareWorksheet.Name & "'!$A$" & iSerieRow, oWorksheet.Range(sColId & "2:" & sColId & iRowIndex)
    End If
End Function

Private Function AddChart(oChartObjects As ChartObjects, iChartIndex As Integer, sChartName As String) As Chart
    Dim oChart As Chart
    Dim oChartObject As ChartObject
    Dim dChartLeft As Double
    Dim dChartTop As Double
    Dim dChartWidth As Double
    Dim dChartHeight As Double
    Dim dChartRowSpace As Double
    Dim dChartColSpace As Double
    Dim dColWidth As Double
    Dim dRowHeight As Double
    Dim iChartCols As Integer
    Dim iChartRow As Integer
    Dim iChartCol As Integer
    Dim iChartColSpan As Integer

    iChartCols = 3

    Select Case iChartCols
        Case Is = 1
            iChartColSpan = 10
        Case Is = 2
            iChartColSpan = 10
        Case Is = 3
            iChartColSpan = 7
        Case Is = 4
            iChartColSpan = 5
    End Select

    dColWidth = 48
    dRowHeight = 15

    dChartRowSpace = 1 * dRowHeight
    dChartColSpace = 1 * dColWidth
    dChartWidth = iChartColSpan * dColWidth
    dChartHeight = 20 * dRowHeight

    iChartCol = (iChartIndex - 1) Mod iChartCols
    iChartRow = (iChartIndex - 1 - iChartCol) / iChartCols

    dChartLeft = 2 * dColWidth + iChartCol * (dChartWidth + dChartColSpace)
    dChartTop = 2 * dRowHeight + iChartRow * (dChartHeight + dChartRowSpace)

    If iChartIndex <= oChartObjects.Count Then
        Set oChartObject = oChartObjects(iChartIndex)
    Else
        Set oChartObject = oChartObjects.Add(Left:=dChartLeft, Top:=dChartTop, Width:=dChartWidth, Height:=dChartHeight)
    End If

    Set oChart = oChartObject.Chart

    With oChartObject
        .Name = sChartName
        .Left = dChartLeft
        .Top = dChartTop
        .Width = dChartWidth
        .Height = dChartHeight
    End With

    With oChart
        .Type = xlLine
        .HasTitle = True
        .ChartTitle.Text = sChartName

        With .Axes(xlCategory)
            .HasTitle = True
            With .AxisTitle
                .Caption = "samples"
                .Font.Size = 10
            End With
        End With

        With .Axes(xlValue)
            .HasTitle = True
            With .AxisTitle
                .Caption = "ms"
                .Font.Size = 10
            End With
        End With
    End With

    Set AddChart = oChart
End Function

Private Function AddPyCharts(oCompareWorksheet As Worksheet, iChartIndex As Integer) As Integer
    Dim oPlainWorksheet As Worksheet
    Dim oSol2Worksheet As Worksheet
    Dim oPythonWorksheet As Worksheet
    Dim oChartObjects As ChartObjects
    Dim oChart As Chart
    Dim I as Integer

    iChartIndex = iChartIndex - 1

    Set oPlainWorksheet = Worksheets("plain-c")
    Set oSol2Worksheet = Worksheets("sol2")
    Set oPythonWorksheet = Worksheets("python")

    Set oChartObjects = oCompareWorksheet.ChartObjects

    iChartIndex = iChartIndex + 1
    Set oChart = AddChart(oChartObjects, iChartIndex, "calling new 400000 times on a 20x20 matrix")
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 1, oPlainWorksheet, "calling new 400000 times on a 20x20 matrix", "plain-c"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 2, oSol2Worksheet, "calling new 400000 times on a 20x20 matrix", "sol2"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 3, oPythonWorksheet, "calling new 400000 times on a 20x20 matrix", "python"
    For I = oChart.SeriesCollection.Count To 4 Step -1
        oChart.SeriesCollection(I).Delete
    Next

    iChartIndex = iChartIndex + 1
    Set oChart = AddChart(oChartObjects, iChartIndex, "mat[i, j, k] 3932160 times")
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 1, oPlainWorksheet, "mat(i, j, k) 3932160 times", "plain-c (i, j, k)"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 2, oSol2Worksheet, "mat(i, j, k) 3932160 times", "sol2 (i, j, k)"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 3, oPlainWorksheet, "mat[{i, j, k}] 3932160 times", "plain-c [{i, j, k}]"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 4, oSol2Worksheet, "mat[{i, j, k}] 3932160 times", "sol2 [{i, j, k}]"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 5, oPythonWorksheet, "mat[i, j, k] 3932160 times", "python [i, j, k]"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 6, oPlainWorksheet, "bytes[i * cols * channels + j * channels + k] 3932160 times", "ffi uchar*"
    For I = oChart.SeriesCollection.Count To 7 Step -1
        oChart.SeriesCollection(I).Delete
    Next

    iChartIndex = iChartIndex + 1
    Set oChart = AddChart(oChartObjects, iChartIndex, "View a Region Of Interest")
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 1, oPlainWorksheet, "calling new 400000 times on a 20x20 matrix", "plain-c :new(roi)"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 2, oPlainWorksheet, "slicing 400000 times on a 20x20 matrix", "plain-c [roi]"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 3, oPythonWorksheet, "calling new 400000 times on a 20x20 matrix", "python [roi]"
    For I = oChart.SeriesCollection.Count To 4 Step -1
        oChart.SeriesCollection(I).Delete
    Next

    AddPyCharts = iChartIndex
End Function

Private Function AddImplChart(oPlainWorksheet As Worksheet, oSol2Worksheet As Worksheet, oCompareWorksheet As Worksheet, sColId As String, iChartIndex As Integer)
    Dim sChartName As String
    Dim oChartObjects As ChartObjects
    Dim oChart As Chart

    sChartName = oPlainWorksheet.Range(sColId & "1").Value
    If sChartName = "slicing 400000 times on a 20x20 matrix" Then
        Exit Function
    End If

    Set oChartObjects = oCompareWorksheet.ChartObjects
    Set oChart = AddChart(oChartObjects, iChartIndex, sChartName)

    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 1, oPlainWorksheet, sChartName, "plain-c"
    AddSeriesByColName oCompareWorksheet, oChart.SeriesCollection, 2, oSol2Worksheet, sChartName, "sol2"
End Function

Private Sub GenBtn_Click()
    Dim oPlainWorksheet As Worksheet
    Dim oSol2Worksheet As Worksheet
    Dim oCompareWorksheet As Worksheet
    Dim iColIndex As Integer
    Dim iChartIndex As Integer
    Dim sColId As String

    Set oPlainWorksheet = Worksheets("plain-c")
    Set oSol2Worksheet = Worksheets("sol2")
    Set oCompareWorksheet = Worksheets("compare")

    iChartIndex = AddPyCharts(oCompareWorksheet, 1)

    iColIndex = 1
    Do While oPlainWorksheet.Range(Chr(Asc("A") + iColIndex) & "1").Value <> ""
        sColId = Chr(Asc("A") + iColIndex)
        AddImplChart oPlainWorksheet, oSol2Worksheet, oCompareWorksheet, sColId, iColIndex + iChartIndex
        iColIndex = iColIndex + 1
    Loop
End Sub
