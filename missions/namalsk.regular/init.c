void main()
{
	/*
	  [Namalsk] CE init (offline)
	*/
	CreateHive();
	GetHive().InitOffline();

	/*
	  [Namalsk] Weather init
	   Warning: DO NOT ALTER following values as they are interconnected with other Namalsk-specific systems!
	   To ensure correct functionality, it is necessary to include weaher init AFTER the hive init.
	*/
	Weather weather = GetGame().GetWeather();
	weather.MissionWeather( true );
	weather.GetOvercast().SetLimits( 0.10, 0.30 );
	weather.GetRain().SetLimits( 0.0, 0.0 );
	weather.GetFog().SetLimits( 0.0, 0.0 );
	weather.GetOvercast().SetForecastChangeLimits( 0.1, 0.3 );
	weather.GetOvercast().SetForecastTimeLimits( 1600, 2100 );
	weather.GetOvercast().Set( Math.RandomFloatInclusive( 0.1, 0.2 ), 0, 0 );	// ignored if storage is present
	weather.GetRain().Set( 0, 0, 0 );											// ignored if storage is present
	weather.GetFog().Set( 0, 0, 0 );											// ignored if storage is present
	weather.SetWindMaximumSpeed( 30 );
	weather.SetWindFunctionParams( 0.1, 1.0, 42 );
	weather.SetStorm( 0, 1, 1 );
	weather.SetRainThresholds( 0.0, 1.0, 0 );

	/*
	  [Namalsk] Mission time init
	   after CE init to determine if storage mission type is outside of the required time-frame
	   currently recommended time-frame is:
		11/1 -> 11/30
		keep in mind that gameplay features are tied to the mission date (stored in the storage) and that it SHOULD remain this period!
	   while using:
		day accelerated 6 times (serverTimeAcceleration=6), resulting in an average 78 min of day-time (RL)
		night accelerated 24 times (serverNightTimeAcceleration=4), resulting in an average of 26 min of night-time (RL)
	*/
	int year, month, day, hour, minute;
	GetGame().GetWorld().GetDate( year, month, day, hour, minute );

    if ( ( month < 11 ) || ( month >= 12 ) )
    {
    	year = 2011;
        month = 11;
        day = 1;
		
		GetGame().GetWorld().SetDate( year, month, day, hour, minute );
	}
};

class CustomMission: MissionServer
{
	private const static string m_SpawnLoadoutDirectory = "$profile:SpawnLoadout/"; // root directory for SpawnLoadout
	private const static string m_KitsDirectory = m_SpawnLoadoutDirectory + "Kits/"; // directory for kits loadout text files
	private const static string m_CommonItems = m_SpawnLoadoutDirectory + "CommonItems.txt"; // file for in common items for players

	void CustomMission()
	{
		FileHandle templateFile;

		if (!FileExist(m_SpawnLoadoutDirectory))
		{
			MakeDirectory(m_SpawnLoadoutDirectory);

			// create default CommonItems.txt
			templateFile = OpenFile(m_CommonItems, FileMode.WRITE);
			FPrintln(templateFile, "Hoodie_Black\nCargopants_Black\nUKAssVest_Black#RGD5Grenade-1#RGD5Grenade-2#RGD5Grenade-3#RGD5Grenade-4\nWorkingGloves_Black\nMilitaryBoots_Black\nRag-5 4\nBandageDressing-6");
			CloseFile(templateFile);
		}

		if (!FileExist(m_KitsDirectory))
		{
			MakeDirectory(m_KitsDirectory);

			// create template kit file
			templateFile = OpenFile(m_KitsDirectory + "template.txt", FileMode.WRITE);
			FPrintln(templateFile, "KitchenKnife\nAKM-0#ak_suppressor#ak_woodbttstck_black#ak_railhndgrd#pso1optic/battery9v-7\nPipe\nCombatKnife");
			CloseFile(templateFile);
		}
	}

	override void OnInit()
	{
		super.OnInit();

		// this piece of code is recommended otherwise event system is switched on automatically and runs from default values
		// comment this whole block if NOT using Namalsk Survival
		if ( m_EventManagerServer )
		{
			// enable/disable event system, min time between events, max time between events, max number of events at the same time
			m_EventManagerServer.OnInitServer( true, 550, 1000, 2 );
			// registering events and their probability
			m_EventManagerServer.RegisterEvent( Aurora, 0.85 );
			m_EventManagerServer.RegisterEvent( Blizzard, 0.4 );
			m_EventManagerServer.RegisterEvent( ExtremeCold, 0.4 );
			m_EventManagerServer.RegisterEvent( Snowfall, 0.6 );
			m_EventManagerServer.RegisterEvent( EVRStorm, 0.35 );
			m_EventManagerServer.RegisterEvent( HeavyFog, 0.3 );
		}
	}

	void SetRandomHealth(EntityAI itemEnt)
	{
		if (itemEnt)
		{
			float rndHlt = Math.RandomFloat( 0.50, 0.85 );
			itemEnt.SetHealth01( "", "", rndHlt );
		}
	}
	
	override void StartingEquipSetup( PlayerBase player, bool clothesChosen )
	{
		//Base namalsk code start
		//If you do not wish for them to spawn with a rag, two flares, a heatpack, or the watch delete from here to line: 164.
		EntityAI itemClothing;
		EntityAI itemEnt;
		ItemBase itemBs;
		float rand;
		
		// top
		itemClothing = player.FindAttachmentBySlotName( "Body" );
		if ( itemClothing )
		{
			SetRandomHealth( itemClothing );

			itemEnt = itemClothing.GetInventory().CreateInInventory( "Rag" );
			if ( Class.CastTo( itemBs, itemEnt ) )
				itemBs.SetQuantity( 4 );
			player.SetQuickBarEntityShortcut( itemEnt, 0 );

			SetRandomHealth( itemEnt );
			
			itemEnt = itemClothing.GetInventory().CreateInInventory( "RoadFlare" );
			SetRandomHealth( itemEnt );
			itemEnt = itemClothing.GetInventory().CreateInInventory( "RoadFlare" );
			SetRandomHealth( itemEnt );
			player.SetQuickBarEntityShortcut( itemEnt, 1 );
		}

		// pants
		itemClothing = player.FindAttachmentBySlotName( "Legs" );
		if ( itemClothing )
		{
			SetRandomHealth( itemClothing );

			itemEnt = itemClothing.GetInventory().CreateInInventory( "Heatpack" );
			SetRandomHealth( itemEnt );

			int throwDice = Math.RandomInt( 0, 2 );
			if ( throwDice == 0 )
				itemEnt = itemClothing.GetInventory().CreateInInventory( "dzn_tool_watch" );
			else
				itemEnt = itemClothing.GetInventory().CreateInInventory( "dzn_tool_watch2" );
			player.SetQuickBarEntityShortcut( itemEnt, 2 );
		}

		// shoes
		itemClothing = player.FindAttachmentBySlotName( "Feet" );
		if ( itemClothing )
		{
			SetRandomHealth( itemClothing );
		}
		// bump fresh spawn water and energy values (to compensate for the frozen food and harder-to-get wells)
		player.GetStatWater().Set( 900 );
		player.GetStatEnergy().Set( 1100 );

		//End base namalsk code


		FileHandle donatorFile;
		string line;

		TStringArray paths = FindFilesInLocation(m_KitsDirectory);
		if (paths.Count() < 1)
			return; // welp you're going to spawn fucking naked since there are no kits

		string file = paths.GetRandomElement(); // get random loadout

		Log("StartingEquipSetup:: Path: " + file);

		if (!FileExist(file))
			StartingEquipSetup(player, clothesChosen); // re-roll if file doesn't exist

		SpawnLoadout(player, ReadFileLines(file)); // spawn kit
	}

	private void SpawnLoadout(PlayerBase player, ref TStringArray loadout)
	{
		// creates common items
		TStringArray items = ReadFileLines(m_CommonItems);
		foreach (string item : items)
		{
			if (item.Contains(" ")) // check for space, which signifies a quantity item
			{
				Log("Creating Quanity Common Item: " + item);
				CreateQuantityItem(player, item);
				continue;
			}

			if (item.Contains("#")) // item has attachments
			{
				Log("Creating Item with Attachments: " + item);
				CreateItemWithAttachments(player, item);
				continue;
			}

			if(item.Contains("-"))
			{
				SetHotbarLocation(player, null, item);
				continue;
			}

			Log("Creating Base Item: " + item);
			player.GetInventory().CreateInInventory(item);
		}

		// spawn the loadout
		foreach (string load : loadout)
		{
			if (load.Contains(" ")) // check for space, which signifies a quantity item
			{
				Log("Quanity items detected: " + load);
				CreateQuantityItem(player, load);
				continue;
			}

			if (load.Contains("#")) // item has attachments
			{
				Log("Item with attachment detected: " + load);
				CreateItemWithAttachments(player, load);
				continue;
			}

			if (load.Contains("-")) // item has slot
			{
				Log("Item slot specified");
				SetHotbarLocation(player, null, load);
				continue;
			}

			Log("Base item detected: " + load);
			player.GetInventory().CreateInInventory(load);
		}
	}

	private TStringArray FindFilesInLocation(string folder)
	{
		string path = m_KitsDirectory + "*";
		string fileName;
		int fileAttr;
		TStringArray paths = new TStringArray;

		FindFileHandle fileHandler = FindFile(path, fileName, fileAttr, FindFileFlags.ALL);
		bool foundFile = true;

		while(foundFile)
		{
			if(fileName != "")
			{
				if(fileAttr != FileAttr.DIRECTORY)
				{
					string fullPath = folder + fileName;
					Log("Found Loadout File: " + fileName + " at path: " + fullPath);
					paths.Insert(fullPath);
				}
			}
			foundFile = FindNextFile(fileHandler, fileName, fileAttr);
		}
		Log("Found: " + paths.Count());
		return paths;
	}

	private void CreateQuantityItem(PlayerBase player, string item)
	{
		TStringArray quantity = new TStringArray();
		item.Split(" ", quantity);

		ItemBase quantityItem;

		if(quantity[0].Contains("-"))
		{
			quantityItem = ItemBase.Cast(player.GetInventory().CreateInInventory(quantity[0].Substring(0, quantity[0].IndexOf("-"))));
			SetHotbarLocation(player, quantityItem, item);
		}else
			quantityItem = ItemBase.Cast(player.GetInventory().CreateInInventory(quantity[0]));


		if(quantityItem != null)
		{
			Log("Setting: " + quantityItem.GetType() + ":: Amount: " + quantity[1].ToFloat());
			quantityItem.SetQuantity(quantity[1].ToFloat());
		}

		if(quantityItem == null)
			Log("Quantity Item: " + item + " is not registered as a valid item name.");
	}

	private void CreateItemWithAttachments(PlayerBase player, string value)
	{
		EntityAI baseItem;
		TStringArray items = new TStringArray();
		value.Split("#", items);

		string baseItemName = items[0];

		if(baseItemName.Contains("-"))
		{
			TStringArray data = new TStringArray;
			baseItemName.Split("-", data);
			baseItem = player.GetInventory().CreateInInventory(data[0]);
			SetHotbarLocation(player, baseItem, baseItemName);
		}else
			baseItem = player.GetInventory().CreateInInventory(baseItemName);

		for(int i = 1; i < items.Count(); i++)
		{
			EntityAI attachment;
			string item = items[i];

			if(!item.Contains("-") && !item.Contains("/"))
			{
				Log("No Attachment attachment with no slot: " + item);
				attachment = baseItem.GetInventory().CreateAttachment(item);
				continue;
			}

			if(item.Contains("-") && !item.Contains("/"))
			{
				TStringArray hotBarData = new TStringArray;
				item.Split("-", hotBarData);
				attachment = baseItem.GetInventory().CreateAttachment(hotBarData[0]);
				SetHotbarLocation(player, attachment, hotBarData[0]+ "-" +hotBarData[1]);
				Log("Attachment with slot, and no attachments: " + hotBarData[0] +", " + hotBarData[1] + ", Array: " + hotBarData);
				continue;
			}

			if(item.Contains("/"))
			{
				TStringArray attachmentsOfAttachments = new TStringArray;
				item.Split("/", attachmentsOfAttachments);

				for(int x = 0; x < attachmentsOfAttachments.Count(); x++)
				{
					if(x == 0)
					{
						if(attachmentsOfAttachments[x].Contains("-"))
						{
							TStringArray dataSplit = new TStringArray;
							attachmentsOfAttachments[x].Split("-", dataSplit);
							attachment = baseItem.GetInventory().CreateAttachment(dataSplit[0]);
							SetHotbarLocation(player, attachment, dataSplit[0]+ "-" +dataSplit[1]);
							Log("Attachment of String: " + dataSplit[0] + ", Slot: " + dataSplit[1]);
						}else{
							Log("Attachment of without Slot: " + attachmentsOfAttachments[x]);
							attachment = baseItem.GetInventory().CreateAttachment(attachmentsOfAttachments[x]);
						}
						
						continue;
					}

					if(attachment != null)
					{
						Log("Attaching: " + attachmentsOfAttachments[x] + " to " + attachment.GetType());
						if(attachmentsOfAttachments[x].Contains("-"))
						{
							TStringArray attach = new TStringArray;
							attachmentsOfAttachments[x].Split("-", attach);
							SetHotbarLocation(player, attachment.GetInventory().CreateAttachment(attach[0]), attachmentsOfAttachments[x]);
						}else
							attachment.GetInventory().CreateAttachment(attachmentsOfAttachments[x]);
					}
				}
			}
		}
	}

	private void SetHotbarLocation(PlayerBase player, EntityAI item, string str)
	{
		if(player == null || str == string.Empty)
			return;

		//gun-1#
		TStringArray hotBarData = new TStringArray;
		str.Split("-", hotBarData);

		if(item == null)
		{
			Log("Creating: " + hotBarData[0]);
			item = player.GetInventory().CreateInInventory(hotBarData[0]);
		}

		if(hotBarData.Count() > 0 && item != null)
		{
			string slotStr = hotBarData[1];
			int slot = hotBarData[1].Get(0).ToInt();

			Log("Setting to quickbar slot for " + hotBarData[0] + ", Slot: "+ slot);
			player.SetQuickBarEntityShortcut(item, slot, true);
		}
	}

	private TStringArray ReadFileLines(string path)
	{
		FileHandle file;
		string line;

		TStringArray contents = new TStringArray();

		file = OpenFile(path, FileMode.READ);
		while (FGets(file, line)> 0)
		{
			line.Trim();
			if (line != string.Empty)
			{
				contents.Insert(line);
				line = string.Empty;
			}
		}

		CloseFile(file);
		return contents;
	}

	override void EquipCharacter(MenuDefaultCharacterData char_data)
	{		
		StartingEquipSetup(m_player, true);
	}

	void Log(string msg)
	{
		Print("[SpawnLoadout]:: " + msg); //Comment this line out if you wish to remove print statements to the script log.
	}
};
  
Mission CreateCustomMission(string path)
{
	return new CustomMission();
};