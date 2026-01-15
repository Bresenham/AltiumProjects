{***************************************************************************
  Export schematic components and their pins to JSON (Altium Designer Script)
  - Exports all SCH documents in the focused project
  - Single-part components assumed
  - Does NOT require wires/nets
  - Saves JSON next to the project file (same folder as .PrjPcb)

  Output: <project_folder>\<project_name>_sch_pins.json
***************************************************************************}

procedure ExportSchPinsToJson;
var
  WS       : IWorkspace;
  Proj     : IProject;
  Doc      : IProjectDocument;
  SchDoc   : ISch_Document;

  SchIter  : ISch_Iterator;
  Comp     : ISch_Component;
  PinIter  : ISch_Iterator;
  Pin      : ISch_Pin;

  OutLines : TStringList;

  ProjPath : String;
  OutPath  : String;

  i        : Integer;
  FirstComp: Boolean;
  FirstPin : Boolean;

  function JsonEscape(const S: String): String;
  var
    k : Integer;
    c : Char;
  begin
    Result := '';
    for k := 1 to Length(S) do
    begin
      c := S[k];
      case c of
        '\': Result := Result + '\\';
        '"': Result := Result + '\"';
        #8: Result := Result + '\b';
        #9: Result := Result + '\t';
        #10: Result := Result + '\n';
        #12: Result := Result + '\f';
        #13: Result := Result + '\r';
      else
        Result := Result + c;
      end;
    end;
  end;

  function GetFolderFromPath(const FullPath: String): String;
  var
    p : Integer;
  begin
    Result := FullPath;
    p := LastDelimiter('\', Result);
    if p > 0 then
      Result := Copy(Result, 1, p - 1);
  end;

  function GetFileNameNoExt(const FullPath: String): String;
  var
    p1, p2 : Integer;
    s      : String;
  begin
    s := FullPath;

    p1 := LastDelimiter('\', s);
    if p1 > 0 then
      s := Copy(s, p1 + 1, Length(s) - p1);

    p2 := LastDelimiter('.', s);
    if p2 > 0 then
      s := Copy(s, 1, p2 - 1);

    Result := s;
  end;

  function EnsureBackslash(const Folder: String): String;
  begin
    Result := Folder;
    if (Length(Result) > 0) and (Result[Length(Result)] <> '\') then
      Result := Result + '\';
  end;

  // Tries multiple common properties for project full path
  function GetProjectFullPath(P: IProject): String;
  begin
    Result := '';

    // These property names vary between Altium installs/versions.
    // We try the most common ones; whichever compiles in your environment
    // should be kept, the others can be removed if they error.

    // Try 1: DM_ProjectFullPath
    try
      Result := P.DM_ProjectFullPath;
      if Result <> '' then Exit;
    except
    end;

    // Try 2: DM_FullPath
    try
      Result := P.DM_FullPath;
      if Result <> '' then Exit;
    except
    end;

    // Try 3: DM_ProjectFileName (might be full path or just name)
    try
      Result := P.DM_ProjectFileName;
      if Result <> '' then Exit;
    except
    end;
  end;

begin
  WS := GetWorkspace;
  if WS = nil then
  begin
    ShowMessage('No workspace.');
    Exit;
  end;

  Proj := WS.DM_FocusedProject;
  if Proj = nil then
  begin
    ShowMessage('No focused project. Open/focus the project and try again.');
    Exit;
  end;

  if SchServer = nil then
  begin
    ShowMessage('SchServer is not available.');
    Exit;
  end;

  ProjPath := GetProjectFullPath(Proj);
  if ProjPath = '' then
  begin
    ShowMessage(
      'Could not determine the project full path from this API.' + #13#10 +
      'Tell me which IProject path properties are available in your install, ' +
      'and I will adjust the script.'
    );
    Exit;
  end;

  OutPath :=
    EnsureBackslash(GetFolderFromPath(ProjPath)) +
    GetFileNameNoExt(ProjPath) +
    '_sch_pins.json';

  OutLines := TStringList.Create;
  try
    OutLines.Add('{');
    OutLines.Add('  "projectFile": "' + JsonEscape(ProjPath) + '",');
    OutLines.Add('  "components": [');

    FirstComp := True;

    for i := 0 to Proj.DM_LogicalDocumentCount - 1 do
    begin
      Doc := Proj.DM_LogicalDocuments(i);
      if Doc = nil then
        Continue;

      if Doc.DM_DocumentKind <> 'SCH' then
        Continue;

      SchDoc := SchServer.GetSchDocumentByPath(Doc.DM_FullPath);
      if SchDoc = nil then
      begin
        WS.DM_OpenDocument('SCH', Doc.DM_FullPath);
        SchDoc := SchServer.GetSchDocumentByPath(Doc.DM_FullPath);
      end;

      if SchDoc = nil then
        Continue;

      SchIter := SchDoc.SchIterator_Create;
      try
        SchIter.AddFilter_ObjectSet(MkSet(eSchComponent));

        Comp := SchIter.FirstSchObject;
        while Comp <> nil do
        begin
          if not FirstComp then
            OutLines.Add('    ,');
          FirstComp := False;

          OutLines.Add('    {');
          OutLines.Add(
            '      "sheet": "' + JsonEscape(SchDoc.DocumentName) + '",'
          );
          OutLines.Add(
            '      "designator": "' + JsonEscape(Comp.Designator.Text) + '",'
          );
          OutLines.Add(
            '      "comment": "' + JsonEscape(Comp.Comment.Text) + '",'
          );
          OutLines.Add('      "pins": [');

          FirstPin := True;

          PinIter := Comp.SchIterator_Create;
          try
            PinIter.AddFilter_ObjectSet(MkSet(ePin));

            Pin := PinIter.FirstSchObject;
            while Pin <> nil do
            begin
              if not FirstPin then
                OutLines.Add('        ,');
              FirstPin := False;

              OutLines.Add('        {');
              OutLines.Add(
                '          "num": "' + JsonEscape(Pin.Designator) + '",'
              );
              OutLines.Add(
                '          "name": "' + JsonEscape(Pin.Name) + '"'
              );
              OutLines.Add('        }');

              Pin := PinIter.NextSchObject;
            end;
          finally
            Comp.SchIterator_Destroy(PinIter);
          end;

          OutLines.Add('      ]');
          OutLines.Add('    }');

          Comp := SchIter.NextSchObject;
        end;

      finally
        SchDoc.SchIterator_Destroy(SchIter);
      end;
    end;

    OutLines.Add('  ]');
    OutLines.Add('}');

    OutLines.SaveToFile(OutPath);
    ShowMessage('Exported JSON to:' + #13#10 + OutPath);

  finally
    OutLines.Free;
  end;
end;
