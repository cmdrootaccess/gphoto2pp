#include "helper_camera_wrapper.hpp"

#include "log.h"

#include "helper_gphoto2.hpp"
#include "exceptions.hpp"
#include "camera_list_wrapper.hpp"
#include "camera_wrapper.hpp"
#include "camera_file_wrapper.hpp"
#include "camera_file_path_wrapper.hpp"

#include <sstream>

namespace gphoto2pp
{
	namespace helper
	{
		void capturePreview(CameraWrapper& cameraWrapper, const std::string& outputFilename)
		{
			FILE_LOG(logDEBUG) << "HelperCameraWrapper capturePreview - outputFilename";
			
			CameraFileWrapper cameraFile = cameraWrapper.capturePreview(); // No point in duplicating code, call overloaded method
			
			cameraFile.save(outputFilename);
		}

		void capturePreview(CameraWrapper& cameraWrapper, std::ostream& outputStream)
		{
			FILE_LOG(logDEBUG) << "HelperCameraWrapper capturePreview - outputStream";
			
			CameraFileWrapper cameraFile = cameraWrapper.capturePreview(); // No point in duplicating code, call overloaded method
			
			auto buffer = cameraFile.getDataAndSize();
			
			FILE_LOG(logINFO) << "buffer size: '"<< buffer.size() <<"'";
			
			outputStream.write(buffer.data(), buffer.size());
			
			outputStream.flush();
			// Not sure if I should close the file in here, or let the user do it. I'll let the user do it for now.
		}
		
		void capture(CameraWrapper& cameraWrapper, CameraFileWrapper& cameraFile, bool autoDeleteImageFromSrc /* = false */, const CameraCaptureTypeWrapper& captureType /* = Image */, const CameraFileTypeWrapper& fileType /* = Normal */)
		{
			FILE_LOG(logDEBUG) << "HelperCameraWrapper capture - cameraFile";
			
			auto cameraFilePath = cameraWrapper.capture(captureType);
			
			// This should be the move assignment operator
			cameraFile = cameraWrapper.fileGet(cameraFilePath.Folder, cameraFilePath.Name, fileType);
			
			if(autoDeleteImageFromSrc)
			{
				cameraWrapper.fileDelete(cameraFilePath.Folder, cameraFilePath.Name);
			}
		}
		
		void capture(CameraWrapper& cameraWrapper, const std::string& outputFilename, bool autoDeleteImageFromSrc /* = false */, const CameraCaptureTypeWrapper& captureType /* = Image */, const CameraFileTypeWrapper& fileType /* = Normal */)
		{
			FILE_LOG(logDEBUG) << "HelperCameraWrapper capture - outputFilename";
			
			auto cameraFilePath = cameraWrapper.capture(captureType);
			
			auto cameraFile = cameraWrapper.fileGet(cameraFilePath.Folder, cameraFilePath.Name, fileType);
			
			cameraFile.save(outputFilename);
			
			if(autoDeleteImageFromSrc)
			{
				cameraWrapper.fileDelete(cameraFilePath.Folder, cameraFilePath.Name);
			}
		}

		void capture(CameraWrapper& cameraWrapper, std::ostream& outputStream, bool autoDeleteImageFromSrc /* = false */, const CameraCaptureTypeWrapper& captureType /* = Image */, const CameraFileTypeWrapper& fileType /* = Normal */)
		{
			FILE_LOG(logDEBUG) << "HelperCameraWrapper capture - outputStream";
			
			auto cameraFilePath = cameraWrapper.capture(captureType);
			
			auto cameraFile = cameraWrapper.fileGet(cameraFilePath.Folder, cameraFilePath.Name, fileType);
			
			auto temp = cameraFile.getDataAndSize();
			
			outputStream.write(temp.data(),temp.size());
			
			outputStream.flush(); // If we don't flush, I found strange things might happen to the jpg, for one thing the thumbnail wouldn't show up. Makes sense, as once we leave this scope some items are disposed, so we want to make sure that the stream is flushed.
			// Not sure if I should close the file in here as well, or let the user do it. I'll let the user do it for now.
			
			if(autoDeleteImageFromSrc)
			{
				cameraWrapper.fileDelete(cameraFilePath.Folder, cameraFilePath.Name);
			}
		}
		
		//Private Method
		void getChildrenItems(CameraWrapper& cameraWrapper, const std::string& folder, std::vector<std::string>& allItems, bool getFiles)
		{
			if(getFiles == false)
			{
				allItems.push_back(folder);
			}
			else
			{
				auto cameraListFiles = cameraWrapper.folderListFiles(folder);
				
				for(int i = 0; i < cameraListFiles.count(); ++i)
				{
					allItems.push_back(folder + cameraListFiles.getName(i));
				}
			}
			
			CameraListWrapper cameraListFolders = cameraWrapper.folderListFolders(folder);
			
			for(int i = 0; i < cameraListFolders.count(); ++i)
			{
				getChildrenItems(cameraWrapper, folder + cameraListFolders.getName(i) + "/", allItems, getFiles);
			}
		}
		
		std::vector<std::string> getAllFolders(CameraWrapper& cameraWrapper)
		{
			std::vector<std::string> allFolders;
			
			getChildrenItems(cameraWrapper, "/", allFolders, false);
			
			return allFolders;
		}
		
		std::vector<std::string> getAllFiles(CameraWrapper& cameraWrapper)
		{
			std::vector<std::string> allFiles;
			
			getChildrenItems(cameraWrapper, "/", allFiles, true);
			
			return allFiles;
		}
	}
}