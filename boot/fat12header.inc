bootSectorAdditions:
	; Additonal Information In Boot Sector.
	oemName					db 'LHR'
	times 8-($-oemName)	 		db ' '
	biosParameterBlock:
		bytesPerSector			dw 0200h	
		sectorsPerCluster		db 01h
		reservedSectors			dw 0001h
		numberFats			db 02h
		rootEntries			dw 00e0h
		wordTotalSectors		dw 0b40h
		media				db 0f0h
		wordFatSize			dw 0009h
		sectorsPerTrack			dw 0012h
		heads				dw 0002h
		hiddenSectors			dd 00000000h
		dwordTotalSectors		dd 00000000h
	BiosParameterBlock.end:
	driverNumber				db 00h
	byteReserved				db 00h
	bootSign				db 29h
	volumeId				dd 00000000h
	volumeLabel				db 'LSYS0.01'
	times 11-($-volumeLabel)		db ' '
	fileSystemType				db 'FAT12'
	times 8-($-fileSystemType)		db ' '

	; When Any Of Above Entry Changes, This Section Will Be Recalculated.
	rootEntry.areaBeginSector equ 19
		; = numberFats * wordFatSize + reservedSectors.
	rootEntry.size equ 20h
	rootEntry.count equ 10h
		; = bytesPerSector / rootEntry.size
	rootEntry.areaSize equ 14
		; = ceil(rootEntries * rootEntry.size / bytesPerSector).
	data.areaBeginSector equ 33
		; = rootEntry.areaBeginSector + rootEntry.areaSize
	data.bytesPerCluster equ 0200h
		; = bytesPerSector * sectorsPerCluster

