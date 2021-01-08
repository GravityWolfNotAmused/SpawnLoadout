# SpawnLoadoutThingy

## Configuration File Location
Your configuration directories, and files will be created after adding this custom mission class to your init.c, and running the server once. They are located in `$profiles:SpawnLoadout`.

## Adding new Loadout
Create a text file in `profiles:SpawnLoadout/Kits` you can feel free to name it whatever you wish, as long as it's a txt file type.

## Configuration
In order to understand how to configure a loadout, or your common items you must understand what symbols represent what. Template files use all of the types of markings.

|Delimter|Purpose|Example|
|---|-----|-|
|#|Seperates the item and all of it's attachments|AKM#ak_suppressor#ak_woodbttstck_black#ak_railhndgrd#pso1optic|
|-|Set's Item, Attachment, or attachment of an attachment to a hotbar slot, this value must be a number from 0-9|KitchenKnife-0|
| |Using one space will allow you to specify that it is a quantity item, and how many should be in it's stack.|Rag 4|
|/|Seperates the attachment with the attachments for that attachment. The battery is an attachment for the scope.|AKM#ak_suppressor#ak_woodbttstck_black#ak_railhndgrd#pso1optic/battery9v|

### Notes READ!!!!!:
**If you wish to set a quantity item, and a slot you must specify the slot before the quantity amount. An example of this would be:**

```
Rag-5 4
```

## CommonItems.txt
This file is specifically used to specify which item's all kits/player should receive.

Special Thanks:
[WarDog](https://github.com/wrdg)
