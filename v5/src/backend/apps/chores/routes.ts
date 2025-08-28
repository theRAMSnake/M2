import { Router } from 'express';
import { ChoresService } from './service';
import { AuthRequest, ChoreRequest } from '../../types';
import { 
  handleServiceError, 
  generateId, 
  createValidationMiddleware, 
  sendSuccessResponse, 
  sendCreatedResponse,
  checkAuthorization 
} from '../../utils/routeHelpers';

const router = Router();
const choresService = new ChoresService();

// Validation middleware for chore data
const validateChoreData = createValidationMiddleware(['title', 'is_urgent', 'color', 'is_done']);

// GET /api/chores - Get all chores (personal + family)
router.get('/', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const chores = await choresService.getChores(req.user!.id);
    sendSuccessResponse(res, { chores });
  } catch (error) {
    handleServiceError(error, res, 'getting chores');
  }
});

// GET /api/chores/personal - Get personal chores only
router.get('/personal', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const chores = await choresService.getPersonalChores(req.user!.id);
    sendSuccessResponse(res, { chores });
  } catch (error) {
    handleServiceError(error, res, 'getting personal chores');
  }
});

// GET /api/chores/family - Get family chores only
router.get('/family', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const chores = await choresService.getFamilyChores();
    sendSuccessResponse(res, { chores });
  } catch (error) {
    handleServiceError(error, res, 'getting family chores');
  }
});

// GET /api/chores/urgent-count - Get count of urgent chores
router.get('/urgent-count', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const count = await choresService.getUrgentChoresCount(req.user!.id);
    sendSuccessResponse(res, { count });
  } catch (error) {
    handleServiceError(error, res, 'getting urgent chores count');
  }
});

// POST /api/chores/personal - Create a new personal chore
router.post('/personal', checkAuthorization, validateChoreData, async (req: AuthRequest, res) => {
  try {
    const choreData: ChoreRequest = req.body;
    const id = generateId(choreData.id);
    
    await choresService.addPersonalChore(req.user!.id, {
      id,
      title: choreData.title,
      is_urgent: choreData.is_urgent,
      color: choreData.color,
      is_done: choreData.is_done,
      reoccurance_period: choreData.reoccurance_period,
    });

    sendCreatedResponse(res, { id }, 'Personal chore created successfully');
  } catch (error) {
    handleServiceError(error, res, 'creating personal chore');
  }
});

// POST /api/chores/family - Create a new family chore
router.post('/family', checkAuthorization, validateChoreData, async (req: AuthRequest, res) => {
  try {
    const choreData: ChoreRequest = req.body;
    const id = generateId(choreData.id);
    
    await choresService.addFamilyChore({
      id,
      title: choreData.title,
      is_urgent: choreData.is_urgent,
      color: choreData.color,
      is_done: choreData.is_done,
      reoccurance_period: choreData.reoccurance_period,
    });

    sendCreatedResponse(res, { id }, 'Family chore created successfully');
  } catch (error) {
    handleServiceError(error, res, 'creating family chore');
  }
});

// PUT /api/chores/personal/:id - Update a personal chore
router.put('/personal/:id', checkAuthorization, validateChoreData, async (req: AuthRequest, res) => {
  try {
    const choreId = req.params.id;
    const choreData: ChoreRequest = req.body;

    await choresService.updatePersonalChore(req.user!.id, choreId, {
      id: choreId,
      title: choreData.title,
      is_urgent: choreData.is_urgent,
      color: choreData.color,
      is_done: choreData.is_done,
      reoccurance_period: choreData.reoccurance_period,
    });

    sendSuccessResponse(res, undefined, 'Personal chore updated successfully');
  } catch (error) {
    handleServiceError(error, res, 'updating personal chore');
  }
});

// PUT /api/chores/family/:id - Update a family chore
router.put('/family/:id', checkAuthorization, validateChoreData, async (req: AuthRequest, res) => {
  try {
    const choreId = req.params.id;
    const choreData: ChoreRequest = req.body;

    await choresService.updateFamilyChore(choreId, {
      id: choreId,
      title: choreData.title,
      is_urgent: choreData.is_urgent,
      color: choreData.color,
      is_done: choreData.is_done,
      reoccurance_period: choreData.reoccurance_period,
    });

    sendSuccessResponse(res, undefined, 'Family chore updated successfully');
  } catch (error) {
    handleServiceError(error, res, 'updating family chore');
  }
});

// DELETE /api/chores/personal/:id - Delete a personal chore
router.delete('/personal/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const choreId = req.params.id;
    
    // Check if chore exists
    const existingChore = await choresService.getPersonalChore(req.user!.id, choreId);
    if (!existingChore) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Personal chore not found'
      });
    }

    await choresService.deletePersonalChore(req.user!.id, choreId);
    sendSuccessResponse(res, undefined, 'Personal chore deleted successfully');
  } catch (error) {
    handleServiceError(error, res, 'deleting personal chore');
  }
});

// DELETE /api/chores/family/:id - Delete a family chore
router.delete('/family/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const choreId = req.params.id;
    
    // Check if chore exists
    const existingChore = await choresService.getFamilyChore(choreId);
    if (!existingChore) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Family chore not found'
      });
    }

    await choresService.deleteFamilyChore(choreId);
    sendSuccessResponse(res, undefined, 'Family chore deleted successfully');
  } catch (error) {
    handleServiceError(error, res, 'deleting family chore');
  }
});

// GET /api/chores/personal/:id - Get a specific personal chore
router.get('/personal/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const choreId = req.params.id;
    const chore = await choresService.getPersonalChore(req.user!.id, choreId);
    
    if (!chore) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Personal chore not found'
      });
    }

    sendSuccessResponse(res, chore);
  } catch (error) {
    handleServiceError(error, res, 'getting personal chore');
  }
});

// GET /api/chores/family/:id - Get a specific family chore
router.get('/family/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const choreId = req.params.id;
    const chore = await choresService.getFamilyChore(choreId);
    
    if (!chore) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Family chore not found'
      });
    }

    sendSuccessResponse(res, chore);
  } catch (error) {
    handleServiceError(error, res, 'getting family chore');
  }
});

export default router;
