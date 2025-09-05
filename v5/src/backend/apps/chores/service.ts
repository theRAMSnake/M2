import { DatabaseService } from '../../storage/database';
import { Chore } from '../../types';
import { DisciplineService } from '../discipline/service';

export class ChoresService {
  private db: DatabaseService;

  constructor() {
    this.db = DatabaseService.getInstance();
  }

  async getChores(userId: string): Promise<Chore[]> {
    try {
      const [personalChores, familyChores] = await Promise.all([
        this.getPersonalChores(userId),
        this.getFamilyChores()
      ]);
      
      return [...personalChores, ...familyChores];
    } catch (error) {
      console.error('Error getting chores:', error);
      return [];
    }
  }

  async getPersonalChores(userId: string): Promise<Chore[]> {
    return this.getChoresFromPath(`/${userId}/chores`);
  }

  async getFamilyChores(): Promise<Chore[]> {
    return this.getChoresFromPath('/shared/chores');
  }

  async getUrgentChoresCount(userId: string): Promise<number> {
    try {
      const allChores = await this.getChores(userId);
      return allChores.filter(chore => chore.is_urgent && !chore.is_done).length;
    } catch (error) {
      console.error('Error getting urgent chores count:', error);
      return 0;
    }
  }

  async addPersonalChore(userId: string, chore: Omit<Chore, 'createdBy' | 'createdAt' | 'updatedAt'>): Promise<void> {
    const newChore: Chore = {
      ...chore,
      createdBy: userId,
      createdAt: new Date(),
      updatedAt: new Date(),
    };
    await this.saveChore(`/${userId}/chores/${chore.id}`, newChore);
  }

  async addFamilyChore(chore: Omit<Chore, 'createdBy' | 'createdAt' | 'updatedAt'>): Promise<void> {
    const newChore: Chore = {
      ...chore,
      createdBy: 'shared',
      createdAt: new Date(),
      updatedAt: new Date(),
    };
    await this.saveChore(`/shared/chores/${chore.id}`, newChore);
  }

  async updatePersonalChore(userId: string, choreId: string, updatedChore: Omit<Chore, 'createdBy' | 'createdAt' | 'updatedAt'>): Promise<void> {
    const existingChore = await this.getPersonalChore(userId, choreId);
    if (!existingChore) {
      throw new Error('Personal chore not found');
    }
    
    const choreToSave = await this.buildChoreForUpdate(updatedChore, choreId, existingChore);
    await this.saveChore(`/${userId}/chores/${choreId}`, choreToSave);
  }

  async updateFamilyChore(choreId: string, updatedChore: Omit<Chore, 'createdBy' | 'createdAt' | 'updatedAt'>): Promise<void> {
    const existingChore = await this.getFamilyChore(choreId);
    if (!existingChore) {
      throw new Error('Family chore not found');
    }
    
    const choreToSave = await this.buildChoreForUpdate(updatedChore, choreId, existingChore);
    await this.saveChore(`/shared/chores/${choreId}`, choreToSave);
  }

  async deletePersonalChore(userId: string, choreId: string): Promise<void> {
    await this.deleteChore(`/${userId}/chores/${choreId}`);
  }

  async deleteFamilyChore(choreId: string): Promise<void> {
    await this.deleteChore(`/shared/chores/${choreId}`);
  }

  async getPersonalChore(userId: string, choreId: string): Promise<Chore | null> {
    return this.getChoreFromPath(`/${userId}/chores/${choreId}`);
  }

  async getFamilyChore(choreId: string): Promise<Chore | null> {
    return this.getChoreFromPath(`/shared/chores/${choreId}`);
  }

  // Private helper methods
  private async getChoresFromPath(path: string): Promise<Chore[]> {
    try {
      const documents = await this.db.listDocuments(path);
      const chores: Chore[] = [];
      
      for (const doc of documents) {
        try {
          const choreData = await this.db.readDocument(doc.path);
          if (choreData) {
            const processedChore = await this.processChoreReoccurance(choreData as Chore);
            if (processedChore) {
              chores.push(processedChore);
            }
          }
        } catch (error) {
          console.error(`Error reading chore document ${doc.path}:`, error);
        }
      }
      
      return chores;
    } catch (error) {
      console.error(`Error getting chores from path ${path}:`, error);
      return [];
    }
  }

  private async getChoreFromPath(path: string): Promise<Chore | null> {
    try {
      const choreData = await this.db.readDocument(path);
      if (choreData) {
        return await this.processChoreReoccurance(choreData as Chore);
      }
      return null;
    } catch (error) {
      console.error(`Error getting chore from path ${path}:`, error);
      return null;
    }
  }

  private async saveChore(path: string, chore: Chore): Promise<void> {
    try {
      const choreWithMetadata = {
        ...chore,
        updatedAt: new Date(),
        createdAt: chore.createdAt || new Date(),
      };
      await this.db.writeDocument(path, choreWithMetadata);
    } catch (error) {
      console.error(`Error saving chore at ${path}:`, error);
      throw error;
    }
  }

  private async deleteChore(path: string): Promise<void> {
    try {
      await this.db.deleteDocument(path);
    } catch (error) {
      console.error(`Error deleting chore at ${path}:`, error);
      throw error;
    }
  }

  private async buildChoreForUpdate(
    updatedChore: Omit<Chore, 'createdBy' | 'createdAt' | 'updatedAt'>, 
    choreId: string, 
    existingChore: Chore
  ): Promise<Chore> {
    const choreToSave: Chore = { 
      ...updatedChore, 
      id: choreId, 
      createdBy: existingChore.createdBy,
      createdAt: existingChore.createdAt,
      updatedAt: new Date(),
      hasBeenRewarded: existingChore.hasBeenRewarded, // Preserve existing reward status
    };
    
    // Handle reoccurance logic when chore is marked as done
    if (updatedChore.is_done && updatedChore.reoccurance_period && !existingChore.is_done) {
      const undoneDate = new Date();
      undoneDate.setDate(undoneDate.getDate() + updatedChore.reoccurance_period);
      choreToSave.undone_from = undoneDate.toISOString();
    }
    
    // Don't clear reward flag when manually undone - only clear on respawn
    // The reward flag should persist even if manually undone
    
    // Reward coins when chore is marked as done (but wasn't done before and hasn't been rewarded yet)
    // Default hasBeenRewarded to false for existing documents (backward compatibility)
    const hasBeenRewarded = existingChore.hasBeenRewarded ?? false;
    if (updatedChore.is_done && !existingChore.is_done && !hasBeenRewarded) {
      try {
        const disciplineService = new DisciplineService();
        await disciplineService.rewardChoreCompletion(updatedChore.color, updatedChore.title);
        choreToSave.hasBeenRewarded = true; // Mark as rewarded
      } catch (error) {
        console.error('Error rewarding chore completion:', error);
        // Don't fail the chore update if discipline reward fails
      }
    }
    
    return choreToSave;
  }

  private async processChoreReoccurance(chore: Chore): Promise<Chore | null> {
    try {
      // Check if chore should be undone based on reoccurance
      if (chore.is_done && chore.undone_from && chore.reoccurance_period) {
        const undoneDate = new Date(chore.undone_from);
        const now = new Date();
        
        if (now >= undoneDate) {
          // Undo the chore and clear reward flag for next completion cycle
          const updatedChore: Chore = {
            ...chore,
            is_done: false,
            undone_from: undefined,
            hasBeenRewarded: false, // Clear reward flag for next completion
            updatedAt: new Date(),
          };
          
          // Save the updated chore
          const path = chore.createdBy === 'shared' 
            ? `/shared/chores/${chore.id}` 
            : `/${chore.createdBy}/chores/${chore.id}`;
          
          await this.saveChore(path, updatedChore);
          return updatedChore;
        }
      }
      
      return chore;
    } catch (error) {
      console.error('Error processing chore reoccurance:', error);
      return chore; // Return original chore if processing fails
    }
  }
}
