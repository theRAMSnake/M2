import { Router } from 'express';
import { DatabaseService } from '../../storage/database';
import { AuthRequest } from '../../types';
import { handleServiceError, sendSuccessResponse, checkAuthorization } from '../../utils/routeHelpers';

const router = Router();
const dbService = DatabaseService.getInstance();

// GET /api/stickies/count - Get count of stickies
router.get('/count', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const stickiesDoc = await dbService.readDocument('/shared/stickies');
    console.log('Stickies document from DB:', stickiesDoc);
    let count = 0;
    
    if (stickiesDoc) {
      try {
        // Handle both string and object formats
        const stickiesData = typeof stickiesDoc === 'string' ? JSON.parse(stickiesDoc) : stickiesDoc;
        console.log('Parsed stickies data:', stickiesData);
        
        // Check if stickiesData is an object with stickies array
        if (stickiesData && typeof stickiesData === 'object' && Array.isArray(stickiesData.stickies)) {
          count = stickiesData.stickies.length;
        } else if (Array.isArray(stickiesData)) {
          // Handle case where stickiesData is directly an array
          count = stickiesData.length;
        } else {
          count = 0;
        }
        
        console.log('Calculated count:', count);
      } catch (parseError) {
        console.error('Failed to parse stickies document:', parseError);
        count = 0;
      }
    }
    
    console.log('Final count being sent:', count);
    sendSuccessResponse(res, { count });
  } catch (error) {
    handleServiceError(error, res, 'getting stickies count');
  }
});

// GET /api/stickies - Get stickies data
router.get('/', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const stickiesDoc = await dbService.readDocument('/shared/stickies');
    if (stickiesDoc) {
      const stickiesData = typeof stickiesDoc === 'string' ? JSON.parse(stickiesDoc) : stickiesDoc;
      sendSuccessResponse(res, stickiesData);
    } else {
      sendSuccessResponse(res, { stickies: [] });
    }
  } catch (error) {
    handleServiceError(error, res, 'loading stickies');
  }
});

// POST /api/stickies - Save stickies data
router.post('/', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const stickiesData = req.body;
    await dbService.writeDocument('/shared/stickies', stickiesData);
    sendSuccessResponse(res, { message: 'Stickies saved successfully' });
  } catch (error) {
    handleServiceError(error, res, 'saving stickies');
  }
});

// Temporary debug endpoint to see what's in the database
router.get('/debug', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const stickiesDoc = await dbService.readDocument('/shared/stickies');
    res.json({
      success: true,
      data: {
        documentExists: !!stickiesDoc,
        rawDocument: stickiesDoc,
        parsedDocument: stickiesDoc ? JSON.parse(stickiesDoc) : null
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      error: error instanceof Error ? error.message : 'Unknown error'
    });
  }
});

export default router;
