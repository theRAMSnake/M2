import { DatabaseService } from '../../storage/database';

export interface BusinessEvent {
  id: string;
  date: string;
  amount: number;
  category: 'Client A' | 'Client B' | 'Ads cost' | 'Other cost';
  description?: string;
}

export class MakeUpService {
  private db: DatabaseService;

  constructor() {
    this.db = DatabaseService.getInstance();
  }

  async getBusinessEvents(userId: string): Promise<BusinessEvent[]> {
    try {
      const path = `/${userId}/business_events`;
      const documents = await this.db.listDocuments(path);
      
      const events: BusinessEvent[] = [];
      for (const doc of documents) {
        try {
          const eventData = await this.db.readDocument(doc.path);
          if (eventData) {
            events.push(eventData as BusinessEvent);
          }
        } catch (error) {
          console.error(`Error reading event document ${doc.path}:`, error);
        }
      }
      
      return events.sort((a, b) => new Date(b.date).getTime() - new Date(a.date).getTime());
    } catch (error) {
      console.error('Error getting business events:', error);
      return [];
    }
  }

  async saveBusinessEvent(userId: string, event: BusinessEvent): Promise<void> {
    try {
      const path = `/${userId}/business_events/${event.id}`;
      await this.db.writeDocument(path, event);
    } catch (error) {
      console.error('Error saving business event:', error);
      throw error;
    }
  }

  async deleteBusinessEvent(userId: string, eventId: string): Promise<void> {
    try {
      const path = `/${userId}/business_events/${eventId}`;
      await this.db.deleteDocument(path);
    } catch (error) {
      console.error('Error deleting business event:', error);
      throw error;
    }
  }

  async addBusinessEvent(userId: string, event: BusinessEvent): Promise<void> {
    try {
      await this.saveBusinessEvent(userId, event);
    } catch (error) {
      console.error('Error adding business event:', error);
      throw error;
    }
  }

  async updateBusinessEvent(userId: string, eventId: string, updatedEvent: BusinessEvent): Promise<void> {
    try {
      // Ensure the event ID matches
      const eventToSave = { ...updatedEvent, id: eventId };
      await this.saveBusinessEvent(userId, eventToSave);
    } catch (error) {
      console.error('Error updating business event:', error);
      throw error;
    }
  }

  async getBusinessEvent(userId: string, eventId: string): Promise<BusinessEvent | null> {
    try {
      const path = `/${userId}/business_events/${eventId}`;
      const eventData = await this.db.readDocument(path);
      return eventData as BusinessEvent || null;
    } catch (error) {
      console.error('Error getting business event:', error);
      return null;
    }
  }
}
