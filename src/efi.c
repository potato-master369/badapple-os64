/* Bad Apple/generic video player for x86_64 EFI (possibly
 * generic?)
 */

#include <efi.h>
#include <efilib.h>

// EFI TIMER IS A DAMN SCAM
static inline uint64_t read_tsc() {
	uint32_t low, high;
	__asm__ __volatile__ ("rdtsc" : "=a" (low), "=d" (high));
     	return ((uint64_t)high << 32) | low;
}	

/* ticks per us */
uint64_t tpus = 0;

void callibrate_tsc(EFI_SYSTEM_TABLE *SystemTable) {
	uint64_t wait_us = 100000;
	
	uint64_t start = read_tsc();

	uefi_call_wrapper(SystemTable->BootServices->Stall, 1, wait_us);
	uint64_t end = read_tsc();
	tpus = (end - start) / wait_us;

	/* if its 0, wtf is going on? */
}

void delay_us(uint64_t microseconds) {
    uint64_t start = read_tsc();
    uint64_t ticks_to_wait = microseconds * tpus;
    while ((read_tsc() - start) < ticks_to_wait) {
        __asm__ __volatile__ ("pause");
    }
}

EFI_STATUS EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  EFI_INPUT_KEY Key;

  /* initialise gop */
  EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
  EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

  InitializeLib (ImageHandle, SystemTable);
  callibrate_tsc(SystemTable);

  Status =
    uefi_call_wrapper (SystemTable->BootServices->LocateProtocol, 3, &GopGuid,
		       NULL, (VOID **) & Gop);
  if (EFI_ERROR (Status))
    {
      Print (L"Error: Unable to locate GOP.\n");
      return Status;
    }
  UINT32 Width = Gop->Mode->Info->HorizontalResolution;
  UINT32 Height = Gop->Mode->Info->VerticalResolution;
  UINT32 Ppsl = Gop->Mode->Info->PixelsPerScanLine;
  UINT32 *FrameBuffer = (UINT32 *) Gop->Mode->FrameBufferBase;

  for (int x = 0; x < Width; ++x)
    {
      for (int y = 0; y < Height; ++y)
	{
	  FrameBuffer[y * Ppsl + x] = 0xFF000098;
	}
    }
  uefi_call_wrapper (SystemTable->ConOut->SetAttribute, 2,
		     SystemTable->ConOut,
		     EFI_BACKGROUND_BLUE | EFI_LIGHTGRAY);
  Print
    (L"\n\n\n\n\n  Bad Apple!!/Generic Video player for x86_64 EFI (YT: @WompWomp1234-j6c/GitHub: potato-master369)\n");
  Print
    (L"\n                                    Press any key to continue...\n");

  uefi_call_wrapper (ST->ConIn->Reset, 2, ST->ConIn, FALSE);

  Status = uefi_call_wrapper (ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
  while (Status == EFI_NOT_READY)
    {
      Status =
	uefi_call_wrapper (ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
    }

  Print
    (L"\nPreparing Video Player...\n=-------------------------------------------=\n");
  Print (L"\nSetting Video Mode (your screen may flicker)... ");
  uint8_t setmode = 0;
  for (int i = 0; i < Gop->Mode->MaxMode; ++i)
    {
      UINTN SizeOfInfo;
      EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;

      Status =
	uefi_call_wrapper (Gop->QueryMode, 4, Gop, i, &SizeOfInfo, &Info);
      if (EFI_ERROR (Status))
	{
	  Print (L"Encountered an error processing a mode. Skipping!\n");
	  continue;
	}

      if (Info->HorizontalResolution == 640
	  && Info->VerticalResolution == 480)
	{
	  Status = uefi_call_wrapper (Gop->SetMode, 2, Gop, i);
	  setmode = 1;
	  FreePool (Info);
	  break;
	}

      FreePool (Info);
    }
  if (setmode == 1)
    {
      uefi_call_wrapper (SystemTable->ConOut->ClearScreen, 1,
			 SystemTable->ConOut);
      Width = Gop->Mode->Info->HorizontalResolution;
      Height = Gop->Mode->Info->VerticalResolution;
      Ppsl = Gop->Mode->Info->PixelsPerScanLine;
      FrameBuffer = (UINT32 *) Gop->Mode->FrameBufferBase;
      for (int x = 0; x < Width; ++x)
	{
	  for (int y = 0; y < Height; ++y)
	    {
	      FrameBuffer[y * Ppsl + x] = 0xFF000098;
	    }
	}
      Print (L"Done\n");
    }
  else
    {
      Print (L"Failed!\n");
      for (;;);
    }

  Print (L"Reading from \\badapple.dat...");
  EFI_LOADED_IMAGE *Li;
  EFI_FILE_IO_INTERFACE *fs;
  EFI_FILE_HANDLE root;
  EFI_FILE_HANDLE badapple_frames;

  Status = uefi_call_wrapper (SystemTable->BootServices->OpenProtocol, 6,
			      ImageHandle, &gEfiLoadedImageProtocolGuid,
			      (VOID **) & Li,
			      ImageHandle, NULL,
			      EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if (EFI_ERROR (Status))
    {
      Print (L"Failed!\nFailed to open LoadedImage protocol!");
      for (;;);
    }

  Status = uefi_call_wrapper (SystemTable->BootServices->OpenProtocol, 6,
			      Li->DeviceHandle,
			      &gEfiSimpleFileSystemProtocolGuid,
			      (VOID **) & fs,
			      ImageHandle,
			      NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if (EFI_ERROR (Status))
    {
      Print (L"Failed!\nFailed to open SimpleFileSystem protocol!");
      for (;;);
    }

  Status = uefi_call_wrapper (fs->OpenVolume, 2, fs, &root);
  if (EFI_ERROR (Status))
    {
      Print (L"Failed!\nFailed to open Root Volume (\\)!");
      /* uefi_call_wrapper(root->Close, 1, RootDir); */
      for (;;);
    }

  Status = uefi_call_wrapper (root->Open, 5,
			      root,
			      &badapple_frames,
			      L"badapple.dat", EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status))
    {
      Print (L"Failed!\nFailed to open \\badapple.dat!");
      uefi_call_wrapper (root->Close, 1, root);
      for (;;);
    }

  EFI_FILE_INFO *FileInfo;
  UINTN FileSize;
  VOID *FBuf;

  FileInfo = LibFileInfo (badapple_frames);
  /* JUST TO BE SAFEEEE */
  if (!FileInfo)
    {
      uefi_call_wrapper (badapple_frames->Close, 1, badapple_frames);
      uefi_call_wrapper (root->Close, 1, root);
      Print (L"Failed!\n LibFileInfo failed!");
      for (;;);
    }
  FileSize = FileInfo->FileSize;

  Status =
    uefi_call_wrapper (BS->AllocatePool, 3, EfiLoaderData, FileSize, &FBuf);
  if (EFI_ERROR (Status))
    {
      uefi_call_wrapper (badapple_frames->Close, 1, badapple_frames);
      uefi_call_wrapper (root->Close, 1, root);
      Print (L"Failed!\n AllocatePool failed!");
      for (;;);
    }

  Status =
    uefi_call_wrapper (badapple_frames->Read, 3, badapple_frames, &FileSize,
		       FBuf);
  FreePool (FileInfo);
  uefi_call_wrapper (badapple_frames->Close, 1, badapple_frames);
  uefi_call_wrapper (root->Close, 1, root);
  Print (L"Done\n");

  Print (L"Creating shadow buffer...");

  VOID *BlitBuffer = NULL;
  Status =
    uefi_call_wrapper (BS->AllocatePool, 3, EfiLoaderData, 480 * Ppsl * 4,
		       &BlitBuffer);
  if (EFI_ERROR (Status))
    {
      Print (L"Failed\nFailed to allocate shadow buffer");
      for (;;);
    }

  Print (L"Done\n");
  uint8_t *framedata = (uint8_t *) FBuf;
  UINTN Bf = (640 / 8) * 480;	/* Adjust math if needed */
  Print (L"Starting!\n");

  for (UINTN i = 0; i < 4643; ++i)
    {
      uint8_t *now = framedata + (i * Bf);
      uint32_t *BaseBlitPtr = (uint32_t *) BlitBuffer;
      uint64_t start = read_tsc();

      for (int y = 0; y < 480; ++y)
	{
	  uint32_t *rowptr = BaseBlitPtr + (y * Ppsl);
	  for (int x = 0; x < 640; x += 8)
	    {
	      uint8_t pixel_byte = *now++;
	      rowptr[x + 0] = (pixel_byte & 0x80) ? 0xFFFFFFFF : 0xFF000000;
	      rowptr[x + 1] = (pixel_byte & 0x40) ? 0xFFFFFFFF : 0xFF000000;
	      rowptr[x + 2] = (pixel_byte & 0x20) ? 0xFFFFFFFF : 0xFF000000;
	      rowptr[x + 3] = (pixel_byte & 0x10) ? 0xFFFFFFFF : 0xFF000000;
	      rowptr[x + 4] = (pixel_byte & 0x08) ? 0xFFFFFFFF : 0xFF000000;
	      rowptr[x + 5] = (pixel_byte & 0x04) ? 0xFFFFFFFF : 0xFF000000;
	      rowptr[x + 6] = (pixel_byte & 0x02) ? 0xFFFFFFFF : 0xFF000000;
	      rowptr[x + 7] = (pixel_byte & 0x01) ? 0xFFFFFFFF : 0xFF000000;
	    }
	}

      Status = uefi_call_wrapper (Gop->Blt, 10, Gop, BlitBuffer, EfiBltBufferToVideo,
           0, 0, 0, 0, 640, 480, Ppsl * 4);
      uint64_t time = read_tsc() - start;
      time = time / tpus;

      if (time < 50000) {
          delay_us(50000 - time);
      }


    }

  FreePool (FBuf);
  Status = uefi_call_wrapper (gBS->FreePool, 1, BlitBuffer);
  Print (L"Done playing. YOU MAY POWER YOUR DEVICE OFF NOW.\n");
  for (;;);

  return EFI_SUCCESS;
}
